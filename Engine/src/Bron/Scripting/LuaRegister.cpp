//
// Created by mathi on 19-9-2026.
//

#include "LuaRegister.h"

#include "Bron/Core/Input.h"
#include "Bron/Core/KeyCodes.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/MouseCodes.h"
#include "Bron/Scene/Components.h"
#include "Bron/Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace bron::lua {

namespace {
// False once the entity is destroyed, or queued for destruction by a script.
bool IsValid(const Scene& scene, const Entity& entity) {
	return scene.reg.valid(entity.handle) && !scene.reg.all_of<PendingDestroyComponent>(entity.handle);
}

// The scene's registry, or a Lua error when the entity is gone. Every binding that
// touches an entity goes through here, so a script holding on to a destroyed entity
// gets an error rather than reading a recycled slot.
entt::registry& Registry(Scene& scene, const Entity& entity) {
	if (!IsValid(scene, entity))
		throw sol::error("attempt to use a destroyed entity");
	return scene.reg;
}

void MarkForDestroy(entt::registry& reg, const entt::entity entity) {
	reg.emplace_or_replace<PendingDestroyComponent>(entity);
	for (const entt::entity child: reg.get<HierarchyComponent>(entity).children)
		MarkForDestroy(reg, child);
}

// ============================================================
// Math helpers
// ============================================================

// What vec2, vec3 and vec4 have in common. The scalar overloads go both ways so
// `v * 2` and `2 * v` both work.
template<typename V>
void AddVectorOperations(sol::usertype<V>& type) {
	type[sol::meta_function::addition] = [](const V& a, const V& b) -> V { return a + b; };
	type[sol::meta_function::subtraction] = [](const V& a, const V& b) -> V { return a - b; };
	type[sol::meta_function::unary_minus] = [](const V& v) -> V { return -v; };
	type[sol::meta_function::multiplication] = sol::overload([](const V& a, const V& b) -> V { return a * b; },
															 [](const V& v, const float s) -> V { return v * s; },
															 [](const float s, const V& v) -> V { return s * v; });
	type[sol::meta_function::division] = sol::overload([](const V& a, const V& b) -> V { return a / b; },
													   [](const V& v, const float s) -> V { return v / s; });
	type[sol::meta_function::equal_to] = [](const V& a, const V& b) { return a == b; };

	type["dot"] = [](const V& a, const V& b) { return glm::dot(a, b); };
	type["length"] = [](const V& v) { return glm::length(v); };
	type["length_squared"] = [](const V& v) { return glm::dot(v, v); };
	type["normalize"] = [](const V& v) -> V { return glm::normalize(v); };
	type["distance"] = [](const V& a, const V& b) { return glm::distance(a, b); };
	type["lerp"] = [](const V& a, const V& b, const float t) -> V { return glm::mix(a, b, t); };
}

// ============================================================
// Component helpers
// ============================================================

// The returned references point into the registry's storage, so writes from Lua
// land on the component directly. They stay valid until an entity with that
// component is destroyed, which is why scripts should fetch components where they
// use them rather than keep them in `self` across frames.

// Components every entity has (Scene::CreateEntity adds them, and the engine reads
// them unchecked), so a script may read them but not add or remove them.
template<typename T>
void BindRequiredComponent(sol::usertype<Entity>& type, Scene* s, const std::string& name) {
	type["get_" + name] = [s](const Entity& self) -> T& { return Registry(*s, self).get<T>(self.handle); };
}

template<typename T>
void BindOptionalComponent(sol::usertype<Entity>& type, Scene* s, const std::string& name) {
	// nil when the entity does not have one.
	type["get_" + name] = [s](const Entity& self) -> T* { return Registry(*s, self).try_get<T>(self.handle); };
	type["has_" + name] = [s](const Entity& self) { return Registry(*s, self).all_of<T>(self.handle); };
	// Returns the existing component when there already is one.
	type["add_" + name] = [s](const Entity& self) -> T& { return Registry(*s, self).get_or_emplace<T>(self.handle); };
	type["remove_" + name] = [s](const Entity& self) { Registry(*s, self).remove<T>(self.handle); };
}

// ============================================================
// Log helpers
// ============================================================

// Replaces Lua's print, which writes to stdout where the editor never shows it.
// Raw C function so it takes any number of arguments of any type, like the original.
int Print(lua_State* state) {
	std::string message;
	const int count = lua_gettop(state);
	for (int i = 1; i <= count; ++i) {
		if (i > 1)
			message += '\t';
		size_t length = 0;
		const char* text = luaL_tolstring(state, i, &length);
		message.append(text, length);
		lua_pop(state, 1);
	}
	BR_CORE_INFO("[lua] {}", message);
	return 0;
}

// ============================================================
// Input helpers
// ============================================================

#define BR_LUA_KEYS(ENTRY)                                                                                             \
	ENTRY(Space)                                                                                                       \
	ENTRY(Apostrophe)                                                                                                  \
	ENTRY(Comma)                                                                                                       \
	ENTRY(Minus)                                                                                                       \
	ENTRY(Period)                                                                                                      \
	ENTRY(Slash)                                                                                                       \
	ENTRY(D0)                                                                                                          \
	ENTRY(D1)                                                                                                          \
	ENTRY(D2)                                                                                                          \
	ENTRY(D3)                                                                                                          \
	ENTRY(D4)                                                                                                          \
	ENTRY(D5)                                                                                                          \
	ENTRY(D6)                                                                                                          \
	ENTRY(D7)                                                                                                          \
	ENTRY(D8)                                                                                                          \
	ENTRY(D9)                                                                                                          \
	ENTRY(Semicolon)                                                                                                   \
	ENTRY(Equal)                                                                                                       \
	ENTRY(A)                                                                                                           \
	ENTRY(B)                                                                                                           \
	ENTRY(C)                                                                                                           \
	ENTRY(D)                                                                                                           \
	ENTRY(E)                                                                                                           \
	ENTRY(F)                                                                                                           \
	ENTRY(G)                                                                                                           \
	ENTRY(H)                                                                                                           \
	ENTRY(I)                                                                                                           \
	ENTRY(J)                                                                                                           \
	ENTRY(K)                                                                                                           \
	ENTRY(L)                                                                                                           \
	ENTRY(M)                                                                                                           \
	ENTRY(N)                                                                                                           \
	ENTRY(O)                                                                                                           \
	ENTRY(P)                                                                                                           \
	ENTRY(Q)                                                                                                           \
	ENTRY(R)                                                                                                           \
	ENTRY(S)                                                                                                           \
	ENTRY(T)                                                                                                           \
	ENTRY(U)                                                                                                           \
	ENTRY(V)                                                                                                           \
	ENTRY(W)                                                                                                           \
	ENTRY(X)                                                                                                           \
	ENTRY(Y)                                                                                                           \
	ENTRY(Z)                                                                                                           \
	ENTRY(LeftBracket)                                                                                                 \
	ENTRY(Backslash)                                                                                                   \
	ENTRY(RightBracket)                                                                                                \
	ENTRY(GraveAccent)                                                                                                 \
	ENTRY(World1)                                                                                                      \
	ENTRY(World2)                                                                                                      \
	ENTRY(Escape)                                                                                                      \
	ENTRY(Enter)                                                                                                       \
	ENTRY(Tab)                                                                                                         \
	ENTRY(Backspace)                                                                                                   \
	ENTRY(Insert)                                                                                                      \
	ENTRY(Delete)                                                                                                      \
	ENTRY(Right)                                                                                                       \
	ENTRY(Left)                                                                                                        \
	ENTRY(Down)                                                                                                        \
	ENTRY(Up)                                                                                                          \
	ENTRY(PageUp)                                                                                                      \
	ENTRY(PageDown)                                                                                                    \
	ENTRY(Home)                                                                                                        \
	ENTRY(End)                                                                                                         \
	ENTRY(CapsLock)                                                                                                    \
	ENTRY(ScrollLock)                                                                                                  \
	ENTRY(NumLock)                                                                                                     \
	ENTRY(PrintScreen)                                                                                                 \
	ENTRY(Pause)                                                                                                       \
	ENTRY(F1)                                                                                                          \
	ENTRY(F2)                                                                                                          \
	ENTRY(F3)                                                                                                          \
	ENTRY(F4)                                                                                                          \
	ENTRY(F5)                                                                                                          \
	ENTRY(F6)                                                                                                          \
	ENTRY(F7)                                                                                                          \
	ENTRY(F8)                                                                                                          \
	ENTRY(F9)                                                                                                          \
	ENTRY(F10)                                                                                                         \
	ENTRY(F11)                                                                                                         \
	ENTRY(F12)                                                                                                         \
	ENTRY(F13)                                                                                                         \
	ENTRY(F14)                                                                                                         \
	ENTRY(F15)                                                                                                         \
	ENTRY(F16)                                                                                                         \
	ENTRY(F17)                                                                                                         \
	ENTRY(F18)                                                                                                         \
	ENTRY(F19)                                                                                                         \
	ENTRY(F20)                                                                                                         \
	ENTRY(F21)                                                                                                         \
	ENTRY(F22)                                                                                                         \
	ENTRY(F23)                                                                                                         \
	ENTRY(F24)                                                                                                         \
	ENTRY(F25)                                                                                                         \
	ENTRY(KP0)                                                                                                         \
	ENTRY(KP1)                                                                                                         \
	ENTRY(KP2)                                                                                                         \
	ENTRY(KP3)                                                                                                         \
	ENTRY(KP4)                                                                                                         \
	ENTRY(KP5)                                                                                                         \
	ENTRY(KP6)                                                                                                         \
	ENTRY(KP7)                                                                                                         \
	ENTRY(KP8)                                                                                                         \
	ENTRY(KP9)                                                                                                         \
	ENTRY(KPDecimal)                                                                                                   \
	ENTRY(KPDivide)                                                                                                    \
	ENTRY(KPMultiply)                                                                                                  \
	ENTRY(KPSubtract)                                                                                                  \
	ENTRY(KPAdd)                                                                                                       \
	ENTRY(KPEnter)                                                                                                     \
	ENTRY(KPEqual)                                                                                                     \
	ENTRY(LeftShift)                                                                                                   \
	ENTRY(LeftControl)                                                                                                 \
	ENTRY(LeftAlt)                                                                                                     \
	ENTRY(LeftSuper)                                                                                                   \
	ENTRY(RightShift)                                                                                                  \
	ENTRY(RightControl)                                                                                                \
	ENTRY(RightAlt)                                                                                                    \
	ENTRY(RightSuper)                                                                                                  \
	ENTRY(Menu)

#define BR_LUA_KEY_ENTRY(name) {#name, key::name},
} // namespace

// ============================================================
// Math
// ============================================================

void RegisterMath(sol::state& state) {
	// glm's default constructors leave the components uninitialised, so every type
	// is built through factories that zero it (or make it identity) instead.

	// vec2
	sol::usertype<glm::vec2> vec2 = state.new_usertype<glm::vec2>(
			"vec2", "new",
			sol::factories([] { return glm::vec2(0.0f); }, [](const float s) { return glm::vec2(s); },
						   [](const float x, const float y) { return glm::vec2(x, y); }),
			"x", &glm::vec2::x, "y", &glm::vec2::y);
	AddVectorOperations(vec2);
	vec2[sol::meta_function::to_string] = [](const glm::vec2& v) { return fmt::format("vec2({}, {})", v.x, v.y); };

	// vec3
	sol::usertype<glm::vec3> vec3 = state.new_usertype<glm::vec3>(
			"vec3", "new",
			sol::factories([] { return glm::vec3(0.0f); }, [](const float s) { return glm::vec3(s); },
						   [](const float x, const float y, const float z) { return glm::vec3(x, y, z); },
						   [](const glm::vec2& xy, const float z) { return glm::vec3(xy, z); }),
			"x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
	AddVectorOperations(vec3);
	vec3[sol::meta_function::to_string] = [](const glm::vec3& v) {
		return fmt::format("vec3({}, {}, {})", v.x, v.y, v.z);
	};
	vec3["cross"] = [](const glm::vec3& a, const glm::vec3& b) { return glm::cross(a, b); };

	// vec4
	sol::usertype<glm::vec4> vec4 = state.new_usertype<glm::vec4>(
			"vec4", "new",
			sol::factories(
					[] { return glm::vec4(0.0f); }, [](const float s) { return glm::vec4(s); },
					[](const float x, const float y, const float z, const float w) { return glm::vec4(x, y, z, w); },
					[](const glm::vec3& xyz, const float w) { return glm::vec4(xyz, w); }),
			"x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w", &glm::vec4::w);
	AddVectorOperations(vec4);
	vec4[sol::meta_function::to_string] = [](const glm::vec4& v) {
		return fmt::format("vec4({}, {}, {}, {})", v.x, v.y, v.z, v.w);
	};

	// quat - stored w, x, y, z like TransformComponent::RotationQuat. Angles are radians.
	state.new_usertype<glm::quat>(
			"quat", "new",
			sol::factories(
					[] { return glm::quat(1.0f, 0.0f, 0.0f, 0.0f); },
					[](const float w, const float x, const float y, const float z) { return glm::quat(w, x, y, z); }),
			"w", &glm::quat::w, "x", &glm::quat::x, "y", &glm::quat::y, "z", &glm::quat::z,

			// Static: quat.from_euler(vec3.new(pitch, yaw, roll)), quat.angle_axis(angle, axis)
			"from_euler", [](const glm::vec3& euler) { return glm::quat(euler); }, "angle_axis",
			[](const float angle, const glm::vec3& axis) { return glm::angleAxis(angle, glm::normalize(axis)); },

			sol::meta_function::multiplication,
			sol::overload([](const glm::quat& a, const glm::quat& b) { return a * b; },
						  [](const glm::quat& q, const glm::vec3& v) { return q * v; }),
			sol::meta_function::equal_to, [](const glm::quat& a, const glm::quat& b) { return a == b; },
			sol::meta_function::to_string,
			[](const glm::quat& q) { return fmt::format("quat({}, {}, {}, {})", q.w, q.x, q.y, q.z); },

			"euler", [](const glm::quat& q) { return glm::eulerAngles(q); }, "length",
			[](const glm::quat& q) { return glm::length(q); }, "normalize",
			[](const glm::quat& q) { return glm::normalize(q); }, "conjugate",
			[](const glm::quat& q) { return glm::conjugate(q); }, "inverse",
			[](const glm::quat& q) { return glm::inverse(q); }, "slerp",
			[](const glm::quat& a, const glm::quat& b, const float t) { return glm::slerp(a, b, t); }, "rotate",
			[](const glm::quat& q, const glm::vec3& v) { return q * v; });

	// mat4
	state.new_usertype<glm::mat4>(
			"mat4", "new",
			sol::factories([] { return glm::mat4(1.0f); }, [](const float diagonal) { return glm::mat4(diagonal); }),

			sol::meta_function::addition, [](const glm::mat4& a, const glm::mat4& b) { return a + b; },
			sol::meta_function::subtraction, [](const glm::mat4& a, const glm::mat4& b) { return a - b; },
			sol::meta_function::multiplication,
			sol::overload([](const glm::mat4& a, const glm::mat4& b) { return a * b; },
						  [](const glm::mat4& m, const glm::vec4& v) { return m * v; },
						  [](const glm::mat4& m, const float s) { return m * s; },
						  [](const float s, const glm::mat4& m) { return s * m; }),
			sol::meta_function::equal_to, [](const glm::mat4& a, const glm::mat4& b) { return a == b; },
			sol::meta_function::to_string, [](const glm::mat4&) { return std::string("mat4(...)"); },

			"inverse", [](const glm::mat4& m) { return glm::inverse(m); }, "transpose",
			[](const glm::mat4& m) { return glm::transpose(m); }, "translation",
			[](const glm::mat4& m) { return glm::vec3(m[3]); }, "transform_point",
			[](const glm::mat4& m, const glm::vec3& p) { return glm::vec3(m * glm::vec4(p, 1.0f)); },
			"transform_direction",
			[](const glm::mat4& m, const glm::vec3& d) { return glm::vec3(m * glm::vec4(d, 0.0f)); });

	// Added to Lua's own math table rather than replacing it, so math.floor, math.pi,
	// math.random and the rest stay available.
	sol::table math = state["math"].get_or_create<sol::table>();

	math["radians"] = [](const float degrees) { return glm::radians(degrees); };
	math["degrees"] = [](const float radians) { return glm::degrees(radians); };
	math["clamp"] = [](const float x, const float min, const float max) { return glm::clamp(x, min, max); };
	math["lerp"] = [](const float a, const float b, const float t) { return glm::mix(a, b, t); };

	math["translate"] = [](const glm::mat4& matrix, const glm::vec3& position) {
		return glm::translate(matrix, position);
	};
	math["scale"] = [](const glm::mat4& matrix, const glm::vec3& scale) { return glm::scale(matrix, scale); };
	math["rotate"] = [](const glm::mat4& matrix, const float angle, const glm::vec3& axis) {
		return glm::rotate(matrix, angle, axis);
	};
	math["look_at"] = [](const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
		return glm::lookAt(eye, center, up);
	};
	math["perspective"] = [](const float fov, const float aspect, const float near_plane, const float far_plane) {
		return glm::perspective(fov, aspect, near_plane, far_plane);
	};
}

// ============================================================
// Log
// ============================================================

void RegisterLog(sol::state& state) {
	// "{}" so braces in a script's message are printed rather than read as a format.
	sol::table log = state.create_named_table("log");
	log["trace"] = [](const std::string& message) { BR_CORE_TRACE("[lua] {}", message); };
	log["info"] = [](const std::string& message) { BR_CORE_INFO("[lua] {}", message); };
	log["warn"] = [](const std::string& message) { BR_CORE_WARN("[lua] {}", message); };
	log["error"] = [](const std::string& message) { BR_CORE_ERROR("[lua] {}", message); };

	lua_pushcfunction(state.lua_state(), &Print);
	lua_setglobal(state.lua_state(), "print");
}

// ============================================================
// Input
// ============================================================

void RegisterInput(sol::state& state) {
	// Read-only tables: key.W, mouse.Left, ...
	state.new_enum<KeyCode>("key", {BR_LUA_KEYS(BR_LUA_KEY_ENTRY)});
	state.new_enum<MouseCode>("mouse", {
											   {"Left", mouse::ButtonLeft},
											   {"Right", mouse::ButtonRight},
											   {"Middle", mouse::ButtonMiddle},
											   {"Button0", mouse::Button0},
											   {"Button1", mouse::Button1},
											   {"Button2", mouse::Button2},
											   {"Button3", mouse::Button3},
											   {"Button4", mouse::Button4},
											   {"Button5", mouse::Button5},
											   {"Button6", mouse::Button6},
											   {"Button7", mouse::Button7},
									   });

	sol::table input = state.create_named_table("input");
	input["is_key_pressed"] = [](const KeyCode key) { return Input::IsKeyPressed(key); };
	input["is_mouse_pressed"] = [](const MouseCode button) { return Input::IsMousePressed(button); };
}

// ============================================================
// Components
// ============================================================

void RegisterComponents(sol::state& state) {
	// Scripts reach components through an entity (entity:get_transform()), never
	// create them standalone.
	state.new_usertype<TagComponent>("Tag", sol::no_constructor, "name", &TagComponent::name);

	state.new_usertype<TransformComponent>(
			"Transform", sol::no_constructor, "position", &TransformComponent::Position, "rotation",
			&TransformComponent::RotationQuat, "scale", &TransformComponent::Scaling,
			// Local matrix; the entity's world_transform() includes the parents.
			"matrix", sol::readonly_property([](TransformComponent& t) -> glm::mat4 { return t.GetMatrix(); }));

	state.new_usertype<VisibilityComponent>("Visibility", sol::no_constructor, "visible",
											&VisibilityComponent::visible);

	state.new_usertype<PointLightComponent>("PointLight", sol::no_constructor, "color", &PointLightComponent::color);
}

// ============================================================
// Entity
// ============================================================

void RegisterEntity(sol::state& state, Scene& scene) {
	Scene* s = &scene;
	sol::usertype<Entity> type = state.new_usertype<Entity>("Entity", sol::no_constructor);

	type["is_valid"] = [s](const Entity& self) { return IsValid(*s, self); };
	type["id"] = sol::readonly_property(
			[s](const Entity& self) { return std::string(Registry(*s, self).get<IDComponent>(self.handle).id.value); });
	type["name"] =
			sol::property([s](const Entity& self) { return Registry(*s, self).get<TagComponent>(self.handle).name; },
						  [s](const Entity& self, const std::string& name) {
							  Registry(*s, self).get<TagComponent>(self.handle).name = name;
						  });

	type[sol::meta_function::equal_to] = [](const Entity& a, const Entity& b) { return a.handle == b.handle; };
	type[sol::meta_function::to_string] = [s](const Entity& self) {
		if (!IsValid(*s, self))
			return std::string("Entity(destroyed)");
		return fmt::format("Entity({})", s->reg.get<TagComponent>(self.handle).name);
	};

	// Components
	BindRequiredComponent<TagComponent>(type, s, "tag");
	BindRequiredComponent<TransformComponent>(type, s, "transform");
	BindRequiredComponent<VisibilityComponent>(type, s, "visibility");
	BindOptionalComponent<PointLightComponent>(type, s, "point_light");

	// Hierarchy - through Scene so the parent and child lists stay in sync.
	type["get_parent"] = [s](const Entity& self) -> sol::optional<Entity> {
		const entt::entity parent = Registry(*s, self).get<HierarchyComponent>(self.handle).parent;
		if (parent == entt::null)
			return sol::nullopt;
		return Entity{parent};
	};
	type["get_children"] = [s](const Entity& self) {
		std::vector<Entity> children;
		for (const entt::entity child: Registry(*s, self).get<HierarchyComponent>(self.handle).children)
			children.push_back(Entity{child});
		return sol::as_table(std::move(children));
	};
	// set_parent(nil) moves the entity back under the scene root.
	type["set_parent"] = [s](const Entity& self, sol::optional<Entity> parent) {
		entt::registry& reg = Registry(*s, self);
		if (self.handle == s->root)
			throw sol::error("the scene root cannot be re-parented");

		entt::entity new_parent = s->root;
		if (parent) {
			Registry(*s, *parent);
			new_parent = parent->handle;
		}

		// Scene::AddChild does not guard against cycles, and one would hang
		// WorldTransform and IsVisible.
		for (entt::entity it = new_parent; it != entt::null; it = reg.get<HierarchyComponent>(it).parent) {
			if (it == self.handle)
				throw sol::error("cannot parent an entity to itself or one of its descendants");
		}

		s->AddChild(new_parent, self.handle);
	};

	type["world_transform"] = [s](const Entity& self) {
		Registry(*s, self);
		return s->WorldTransform(self.handle);
	};
	type["world_position"] = [s](const Entity& self) {
		Registry(*s, self);
		return glm::vec3(s->WorldTransform(self.handle)[3]);
	};
	// Visible only when it and every parent are.
	type["is_visible"] = [s](const Entity& self) {
		Registry(*s, self);
		return s->IsVisible(self.handle);
	};
}

// ============================================================
// Scene
// ============================================================

void RegisterScene(sol::state& state, Scene& scene) {
	Scene* s = &scene;
	sol::table table = state.create_named_table("scene");

	table["get_root"] = [s] { return Entity{s->root}; };

	// Parented to the scene root when no parent is given, so it shows up in the editor.
	table["create_entity"] = [s](const std::string& name, sol::optional<Entity> parent) {
		entt::entity parent_handle = s->root;
		if (parent) {
			Registry(*s, *parent);
			parent_handle = parent->handle;
		}
		return Entity{s->CreateEntity(name, parent_handle)};
	};

	// Destroys the entity and its children at the end of the frame; they stop being
	// valid straight away.
	table["destroy_entity"] = [s](const Entity& entity) {
		entt::registry& reg = Registry(*s, entity);
		if (entity.handle == s->root)
			throw sol::error("the scene root cannot be destroyed");
		MarkForDestroy(reg, entity.handle);
	};

	// First entity with this name, or nil. Which one is unspecified when names repeat.
	table["find_entity"] = [s](const std::string& name) -> sol::optional<Entity> {
		for (auto [entity, tag]: s->reg.view<TagComponent>(entt::exclude<PendingDestroyComponent>).each()) {
			if (tag.name == name)
				return Entity{entity};
		}
		return sol::nullopt;
	};
}

void RegisterAll(sol::state& state, Scene& scene) {
	RegisterMath(state);
	RegisterLog(state);
	RegisterInput(state);
	RegisterComponents(state);
	RegisterEntity(state, scene);
	RegisterScene(state, scene);
}

void FlushPendingDestroys(Scene& scene) {
	const auto view = scene.reg.view<PendingDestroyComponent>();
	const std::vector<entt::entity> doomed(view.begin(), view.end());

	for (const entt::entity entity: doomed) {
		// A child is already gone once its parent has been destroyed.
		if (scene.reg.valid(entity))
			scene.DestroyEntity(entity);
	}
}

} // namespace bron::lua
