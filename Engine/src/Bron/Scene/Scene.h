#pragma once

#include <entt/entity/registry.hpp>

#include <string>

#include "Bron/Scene/Components.h"

#include "Bron/Graphics/LightManagement.h"

namespace bron {

class Scene {
public:
	Scene();
	// Out of line: lua_manager is only forward declared here, and destroying it needs
	// the full type.
	~Scene();

	// Creates an entity with a Tag, a Transform and a Hierarchy. Parents it to 'parent'
	// when one is given, otherwise it is left unparented.
	entt::entity CreateEntity(const std::string& name, entt::entity parent = entt::null);

	// Destroys the entity and everything below it in the hierarchy.
	void DestroyEntity(entt::entity entity);

	void AddChild(entt::entity parent, entt::entity child);
	void RemoveChild(entt::entity parent, entt::entity child);

	// Local transform composed with every parent transform up to the root.
	glm::mat4 WorldTransform(entt::entity entity);

	// Checks all parents for visibility
	bool IsVisible(entt::entity entity);

	// Loads a model from disk and attaches it to the entity.
	entt::entity CreatePhongModel(const std::filesystem::path& path);

	// The entity holding the CameraComponent the scene is meant to be looked through,
	// or entt::null when it has no camera at all.
	//
	// Exactly one camera should be marked primary. When none is, the first one found
	// stands in and a warning is logged: a scene that cannot be looked at is much worse
	// than one looked at from an arbitrary angle, and a shipped game has no editor to
	// fix it in.
	[[nodiscard]] entt::entity PrimaryCamera() const;

	entt::registry reg;
	entt::entity root;

	LightManagement light_management;
};
} // namespace bron
