#pragma once

#include <entt/entity/registry.hpp>

#include <string>

#include "Bron/Scene/Components.h"

#include "Bron/Graphics/Camera.h"
#include "Bron/Graphics/LightManagement.h"

namespace bron {
class Scene {
public:
	Scene();

	// Creates an entity with a Tag, a Transform and a Hierarchy. Parents it to 'parent'
	// when one is given, otherwise it is left unparented.
	entt::entity CreateEntity(const std::string& name, entt::entity parent = entt::null);

	// Destroys the entity and everything below it in the hierarchy.
	void DestroyEntity(entt::entity entity);

	void AddChild(entt::entity parent, entt::entity child);
	void RemoveChild(entt::entity parent, entt::entity child);

	// Local transform composed with every parent transform up to the root.
	glm::mat4 WorldTransform(entt::entity entity);

	// Loads a model from disk and parents it to the root.
	entt::entity CreatePhongModel(const char* name, const char* location);

	entt::entity CreatePointLight(glm::vec3 pos, glm::vec3 color);

	entt::registry reg;
	entt::entity root;

	LightManagement light_management;
	Camera* camera = nullptr;
};
} // namespace bron
