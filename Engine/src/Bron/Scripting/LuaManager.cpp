//
// Created by mathi on 19-9-2026.
//

#include "LuaManager.h"

#include "LuaRegister.h"
#include "Bron/Events/KeyEvent.h"
#include "Bron/Events/MouseEvent.h"
#include "Bron/Scene/Scene.h"
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
	sol::table klass;
	sol::table metatable;
	std::vector<ScriptInstance> instances;

	sol::protected_function on_update, on_start, on_destroy, on_event;
};

struct State {
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
} // namespace

LuaManager::LuaManager(Scene* scene) : scene_(scene), state_(CreateScope<State>()) {
	// Libraries first: RegisterMath adds to the math table open_libraries creates.
	state_->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
	lua::RegisterAll(state_->lua, *scene_);
}

LuaManager::~LuaManager() = default;

UUID LuaManager::RegisterScript(const std::filesystem::path& location) const {
	// First check if a script exists
	if (auto* existing =
				Find(state_->scripts, [&location](const auto& item) { return item.second.location == location; })) {
		return existing->second.id;
	}

	sol::protected_function_result function_result = state_->lua.script_file(location.generic_string());
	if (!function_result.valid()) {
		sol::error err = function_result;
		BR_CORE_ASSERT(false, "Script {}. \nExecution failed with message: {}", location.generic_string(), err.what());
	}

	sol::table klass = function_result;
	sol::table meta = state_->lua.create_table();
	meta["__index"] = klass;

	UUID script_id;
	Script script = Script{.id = script_id,
						   .location = location,
						   .klass = klass,
						   .metatable = meta,
						   .instances = {},
						   .on_update = klass["on_update"],
						   .on_start = klass["on_start"],
						   .on_destroy = klass["on_destroy"],
						   .on_event = klass["on_event"]};
	state_->scripts.emplace(script_id, script);

	return script_id;
}

void LuaManager::AttachScript(UUID script_id, entt::entity entity) const {
	auto* existing =
			Find(state_->scripts[script_id].instances, [&entity](const auto& item) { return item.entity == entity; });
	if (existing) {
		return;
	}

	sol::table instance = state_->lua.create_table();
	instance[sol::metatable_key] = state_->scripts[script_id].metatable;

	Entity lua_entity{.handle = entity};
	instance["entity"] = lua_entity;
	state_->scripts[script_id].instances.emplace_back(script_id, entity, instance);
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

} // namespace bron::lua
