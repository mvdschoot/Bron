//
// Created by mathi on 19-9-2026.
//

#include "LuaManager.h"

#include "LuaRegister.h"
#include "ScriptInput.h"
#include "Bron/Input/KeyEvent.h"
#include "Bron/Input/MouseEvent.h"
#include "Bron/Scene/AssetManager.h"
#include "Bron/Scene/Scene.h"
#include "Bron/Util/Paths.h"
#include "Bron/Util/Util.h"

#include <sol/sol.hpp>

#include <algorithm>
#include <map>
#include <ranges>
#include <vector>

namespace bron::lua {

struct ScriptInstance {
	UUID script_id;
	entt::entity entity;
	sol::table self;
	// Set when a callback raises an error; the instance gets no more calls this session,
	// so a broken on_update does not log the same error every frame.
	bool faulted = false;
};

struct Script {
	UUID id;
	std::filesystem::path location;
	sol::table metatable;
	std::vector<ScriptInstance> instances;

	sol::protected_function on_update, on_start, on_destroy, on_event;
};

struct State {
	// Before lua: the input bindings hold a reference to it, so it has to outlive the
	// Lua state, and members are destroyed in reverse order.
	ScriptInput input;
	sol::state lua;
	std::map<UUID, Script> scripts;
};

namespace {
// Calls one of the script's callbacks on one instance, with the instance as self.
// Errors are logged and fault the instance instead of propagating.
template<typename... Args>
void Call(const Script& script, const sol::protected_function& callback, ScriptInstance& instance, Args&&... args) {
	sol::protected_function_result result = callback(instance.self, std::forward<Args>(args)...);
	if (!result.valid()) {
		const sol::error error = result;
		BR_CORE_ERROR("[lua] {} failed and is disabled for this session:\n{}", script.location.generic_string(),
					  error.what());
		instance.faulted = true;
	}
}

// Calls the callback on every live instance of every script that defines it. Indexed
// rather than range-for, so a callback that attaches a script cannot invalidate the
// iteration by growing the vector.
template<typename... Args>
void CallAll(State& state, sol::protected_function Script::* callback, const Args&... args) {
	for (Script& script: std::views::values(state.scripts)) {
		if (!(script.*callback).valid())
			continue;

		for (size_t i = 0; i < script.instances.size(); ++i) {
			if (!script.instances[i].faulted)
				Call(script, script.*callback, script.instances[i], args...);
		}
	}
}

bool IsDestroyed(const Scene& scene, const entt::entity entity) {
	return !scene.reg.valid(entity) || scene.reg.all_of<PendingDestroyComponent>(entity);
}

// Gives the instances of entities destroyed this frame their on_destroy, then drops
// them. The entity already counts as destroyed to Lua, so on_destroy can only use
// the instance's own fields, not the entity's components.
void RemoveDestroyedInstances(State& state, const Scene& scene) {
	for (Script& script: std::views::values(state.scripts)) {
		for (size_t i = 0; i < script.instances.size(); ++i) {
			ScriptInstance& instance = script.instances[i];
			if (IsDestroyed(scene, instance.entity) && !instance.faulted && script.on_destroy.valid())
				Call(script, script.on_destroy, instance);
		}

		std::erase_if(script.instances,
					  [&scene](const ScriptInstance& instance) { return IsDestroyed(scene, instance.entity); });
	}
}

// One of the callbacks off a script's class table. A callback the script does not define
// is absent rather than wrong, so it comes back empty; one defined as something other
// than a function is content being wrong, so it is reported and then ignored. Fetching it
// as sol::object first matters: converting straight to a function would let sol type
// check a number and panic.
sol::protected_function Callback(const sol::table& klass, const char* name, const std::filesystem::path& location) {
	sol::object value = klass[name];
	if (!value.valid())
		return {};

	if (value.get_type() != sol::type::function) {
		BR_CORE_ERROR("[lua] {}: '{}' is not a function, so it will never be called.", location.generic_string(), name);
		return {};
	}

	return value.as<sol::protected_function>();
}

// The script loaded from 'location', loading and registering it the first time it is
// asked for. Returns nullptr when the file cannot be run, having said why.
Script* FindOrLoad(State& state, const assets::AssetHandle& handle) {
	std::filesystem::path path = assets::AssetManager::Instance().Metadata(handle)->path;
	std::filesystem::path absolute = paths::ResolveAsset(path);
	if (auto* existing =
				Find(state.scripts, [&absolute](const auto& item) { return item.second.location == absolute; }))
		return &existing->second;

	sol::protected_function_result function_result = state.lua.script_file(absolute.generic_string());
	if (!function_result.valid()) {
		const sol::error error = function_result;
		// Not an assert: a missing or broken script is content being wrong, not the
		// engine being wrong, and a shipped game must not die because one file is bad.
		BR_CORE_ERROR("[lua] {} could not be loaded:\n{}", absolute.generic_string(), error.what());
		return nullptr;
	}

	// An empty file, or one that forgets its return, runs perfectly well and hands back no
	// value at all - so the result being valid is not enough to assume a table is there.
	// sol would panic converting it, which takes the process with it, hence the check.
	const sol::optional<sol::table> returned = function_result;
	if (!returned) {
		BR_CORE_ERROR("[lua] {} did not return a table. A script file has to end with 'return <name>'.",
					  absolute.generic_string());
		return nullptr;
	}

	const sol::table klass = *returned;
	sol::table meta = state.lua.create_table();
	meta["__index"] = klass;

	const UUID script_id;
	auto [it, inserted] = state.scripts.emplace(script_id, Script{.id = script_id,
																  .location = absolute,
																  .metatable = meta,
																  .instances = {},
																  .on_update = Callback(klass, "on_update", absolute),
																  .on_start = Callback(klass, "on_start", absolute),
																  .on_destroy = Callback(klass, "on_destroy", absolute),
																  .on_event = Callback(klass, "on_event", absolute)});
	return &it->second;
}
} // namespace

LuaManager::LuaManager(Scene* scene) : scene_(scene), state_(CreateScope<State>()) {
	// Libraries first: RegisterMath adds to the math table open_libraries creates.
	state_->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
	lua::RegisterAll(state_->lua, *scene_, state_->input);
}

LuaManager::~LuaManager() = default;

void LuaManager::AttachScript(const assets::AssetHandle& script_handle, const entt::entity entity) const {
	Script* script = FindOrLoad(*state_, script_handle);
	if (!script)
		return;

	// Attaching twice would give the entity two independent selves, and every callback
	// would run on it twice.
	if (Find(script->instances, [entity](const auto& item) { return item.entity == entity; }))
		return;

	sol::table instance = state_->lua.create_table();
	instance[sol::metatable_key] = script->metatable;

	const Entity lua_entity{.handle = entity};
	instance["entity"] = lua_entity;
	script->instances.emplace_back(script->id, entity, instance);
}

void LuaManager::OnUpdate(Timestep ts) const {
	// Seconds as a plain number; Timestep is not a type Lua knows.
	CallAll(*state_, &Script::on_update, ts.GetSeconds());

	RemoveDestroyedInstances(*state_, *scene_);
	lua::FlushPendingDestroys(*scene_);
}

void LuaManager::OnStart() {
	// Every script is attached by now, so on_start can look up other entities and
	// their scripts.
	CallAll(*state_, &Script::on_start);
}

void LuaManager::OnDestroy() {
	CallAll(*state_, &Script::on_destroy);

	// The loaded classes stay, so starting again does not re-read the files.
	for (Script& script: std::views::values(state_->scripts))
		script.instances.clear();

	lua::FlushPendingDestroys(*scene_);
}

void LuaManager::OnEvent(Event& event) {
	// Scripts get input as a table, e.g. { type = "key_pressed", key = key.W }. It is only
	// built when some instance listens, since mouse movement fires constantly.
	const bool listening = std::ranges::any_of(std::views::values(state_->scripts), [](const Script& script) {
		return script.on_event.valid() && !script.instances.empty();
	});
	if (!listening)
		return;

	sol::table lua_event = state_->lua.create_table();
	switch (event.GetEventType()) {
		case EventType::kKeyPressed:
			lua_event["type"] = "key_pressed";
			lua_event["key"] = static_cast<KeyPressedEvent&>(event).GetKey();
			break;
		case EventType::kKeyReleased:
			lua_event["type"] = "key_released";
			lua_event["key"] = static_cast<KeyReleasedEvent&>(event).GetKey();
			break;
		case EventType::kMouseButtonPressed:
			lua_event["type"] = "mouse_pressed";
			lua_event["button"] = static_cast<MouseButtonPressedEvent&>(event).GetButton();
			break;
		case EventType::kMouseButtonReleased:
			lua_event["type"] = "mouse_released";
			lua_event["button"] = static_cast<MouseButtonReleasedEvent&>(event).GetButton();
			break;
		case EventType::kMouseMoved: {
			const auto& moved = static_cast<MouseMovedEvent&>(event);
			lua_event["type"] = "mouse_moved";
			lua_event["x"] = moved.GetX();
			lua_event["y"] = moved.GetY();
			break;
		}
		case EventType::kMouseScrolled: {
			auto& scrolled = static_cast<MouseScrolledEvent&>(event);
			lua_event["type"] = "mouse_scrolled";
			lua_event["x"] = scrolled.GetOffsetX();
			lua_event["y"] = scrolled.GetOffsetY();
			break;
		}
		default:
			// Window and application events stay with the engine.
			return;
	}

	CallAll(*state_, &Script::on_event, lua_event);
}

void LuaManager::SetReceiveMouseInput(bool receive_input) const { state_->input.receive_mouse_input = receive_input; }

void LuaManager::SetReceiveKeyboardInput(bool receive_input) const {
	state_->input.receive_keyboard_input = receive_input;
}

} // namespace bron::lua
