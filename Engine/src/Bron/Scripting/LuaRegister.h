#pragma once

#include <sol/sol.hpp>

#include <entt/entity/entity.hpp>

namespace bron {
class Scene;

namespace lua {
// What a script holds instead of a raw entt::entity: the handle alone does not say
// which registry it belongs to, and it cannot tell a script the entity is gone.
struct Entity {
	Scene* scene = nullptr;
	entt::entity handle = entt::null;

	// False once the entity is destroyed, or queued for destruction by a script.
	bool IsValid() const;
};

// Marks an entity a script destroyed. Destroying straight away would swap another
// entity's components into the freed slots while scripts still hold references to
// them, so the destruction waits for FlushPendingDestroys.
struct PendingDestroyComponent {};

void RegisterMath(sol::state& state);
void RegisterLog(sol::state& state);
void RegisterInput(sol::state& state);
void RegisterComponents(sol::state& state);
void RegisterEntity(sol::state& state);
void RegisterScene(sol::state& state, Scene& scene);

// Everything above, in dependency order.
void RegisterAll(sol::state& state, Scene& scene);

// Destroys the entities scripts destroyed. Call once all scripts are done for the frame.
void FlushPendingDestroys(Scene& scene);
} // namespace lua
} // namespace bron
