#pragma once

#include "Bron/Input/Event.h"


#include <entt/entity/registry.hpp>

#include <string>

#include "Bron/Scene/Components.h"
#include "Bron/Scene/ComponentRestriction.h"

#include "Bron/Graphics/LightManagement.h"

namespace bron {
class Timestep;

namespace lua {
class LuaManager;
}

class Scene {
public:
	Scene();
	// Out of line: lua_manager is only forward declared here, and destroying it needs
	// the full type.
	~Scene();

	// Creates an entity with a Tag, a Transform and a Hierarchy. Parents it to 'parent'
	// when one is given, otherwise it is left unparented.
	entt::entity Create3DEntity(entt::entity parent = entt::null);
	entt::entity Create2DEntity(entt::entity parent);

	// Destroys the entity and everything below it in the hierarchy.
	void DestroyEntity(entt::entity entity);

	void AddChild(entt::entity parent, entt::entity child);
	void RemoveChild(entt::entity parent, entt::entity child);

	// Local transform composed with every parent transform up to the root.
	glm::mat4 WorldTransform(const entt::entity& entity);

	// Checks all parents for visibility
	bool IsVisible(entt::entity entity);

	// Loads the model at 'path' through the asset manager and instantiates it.
	entt::entity CreateModel(const std::filesystem::path& path, MaterialWorkflow workflow = kPhong);
	entt::entity CreatePointLight();
	entt::entity CreateCanvas();
	entt::entity CreateText(entt::entity parent, assets::AssetHandle font);
	entt::entity CreateBox(entt::entity parent);

	// The entity holding the CameraComponent the scene is meant to be looked through,
	// or entt::null when it has no camera at all.
	//
	// Exactly one camera should be marked primary. When none is, the first one found
	// stands in and a warning is logged: a scene that cannot be looked at is much worse
	// than one looked at from an arbitrary angle, and a shipped game has no editor to
	// fix it in.
	[[nodiscard]] entt::entity PrimaryCamera() const;


	void Copy(Scene& dst) const;

	// Playing the scene
	void OnRuntimeStart();
	void OnUpdate(Timestep ts) const;
	void OnEvent(Event& event) const;

	// Component changes that respect the constraints declared in ComponentTraits. See
	// ComponentRestriction.h for the rules; these only bind them to this scene's registry.
	template<Component T>
	[[nodiscard]] bool CanAdd(const entt::entity e) const {
		return restriction::CanAdd<T>(reg, e);
	}

	template<Component T>
	[[nodiscard]] bool CanRemove(const entt::entity e) const {
		return restriction::CanRemove<T>(reg, e);
	}

	template<Component T, typename... Args>
	T& AddComponent(const entt::entity e, Args&&... args) {
		return restriction::Add<T>(reg, e, std::forward<Args>(args)...);
	}

	template<Component T>
	void RemoveComponent(const entt::entity e) {
		restriction::Remove<T>(reg, e);
	}

	entt::registry reg;
	entt::entity root;

	LightManagement light_management;
	Scope<lua::LuaManager> lua_manager;

private:
	// Creates the entities for a model asset: one per node, carrying the node's transform,
	// with the node's meshes on it - or on children of it, when it has more than one. The
	// entities are copies; changing them leaves the model asset alone. Returns the
	// unparented root, or entt::null when the model cannot be loaded.
	entt::entity InstantiateModel(const assets::AssetHandle& model);

	// Set name helper. Not to be used externally. The TagComponent does not get any special treatment.
	void SetName(entt::entity e, const std::string name) { reg.get<TagComponent>(e).name = name; };
};
} // namespace bron
