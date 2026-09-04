#include "Scene.h"

#include <algorithm>
#include <cstdlib>

#include "Bron/Graphics/Components/ModelLoader.h"
#include "Bron/Util/Paths.h"

namespace Bron
{
	Scene::Scene() : lightManagement(*this)
	{
		root = CreateEntity("Root node");
	}

	entt::entity Scene::CreateEntity(const std::string& name, const entt::entity parent)
	{
		const entt::entity entity = reg.create();

		reg.emplace<IDComponent>(entity);
		reg.emplace<TagComponent>(entity, name);
		reg.emplace<TransformComponent>(entity);
		reg.emplace<HierarchyComponent>(entity);

		if (parent != entt::null) {
			AddChild(parent, entity);
		}

		return entity;
	}

	void Scene::DestroyEntity(const entt::entity entity)
	{
		CORE_ASSERT(reg.valid(entity), "Cannot destroy an entity that does not exist")

		HierarchyComponent& hierarchy = reg.get<HierarchyComponent>(entity);

		// Detach from the parent first, so the recursion below cannot walk back up into it.
		if (hierarchy.parent != entt::null) {
			RemoveChild(hierarchy.parent, entity);
		}

		// Copy, because destroying a child mutates the vector we would otherwise be iterating.
		const std::vector<entt::entity> children = hierarchy.children;
		for (const entt::entity child : children) {
			DestroyEntity(child);
		}

		reg.destroy(entity);
	}

	void Scene::AddChild(const entt::entity parent, const entt::entity child)
	{
		CORE_ASSERT(reg.valid(parent) && reg.valid(child), "Cannot parent an entity that does not exist")
		CORE_ASSERT(parent != child, "An entity cannot be its own parent")

		HierarchyComponent& childHierarchy = reg.get<HierarchyComponent>(child);

		// Re-parenting: unhook from the previous parent first.
		if (childHierarchy.parent != entt::null) {
			RemoveChild(childHierarchy.parent, child);
		}

		childHierarchy.parent = parent;
		reg.get<HierarchyComponent>(parent).children.push_back(child);
	}

	void Scene::RemoveChild(const entt::entity parent, const entt::entity child)
	{
		std::vector<entt::entity>& children = reg.get<HierarchyComponent>(parent).children;

		const auto it = std::find(children.begin(), children.end(), child);
		CORE_ASSERT(it != children.end(), "Entity is not a child of this parent")

		children.erase(it);
		reg.get<HierarchyComponent>(child).parent = entt::null;
	}

	glm::mat4 Scene::WorldTransform(const entt::entity entity)
	{
		BR_PROFILE_FUNCTION();

		glm::mat4 transform = *reg.get<TransformComponent>(entity);

		entt::entity parent = reg.get<HierarchyComponent>(entity).parent;
		while (parent != entt::null) {
			transform = *reg.get<TransformComponent>(parent) * transform;
			parent = reg.get<HierarchyComponent>(parent).parent;
		}

		return transform;
	}

	entt::entity Scene::CreatePhongModel(const char* name, const char* location)
	{
		const entt::entity model = ModelLoader::loadModel(*this, MaterialWorkflow::PHONG, location);

		reg.get<TagComponent>(model).name = name;

		// Recorded relative to the asset root so a save file survives the project
		// being moved. A path outside the root is kept as it is; joining an
		// absolute path back onto the root is a no-op, so loading still works.
		std::error_code ec;
		const std::filesystem::path relative = std::filesystem::relative(location, Paths::ProjectAssetRoot(), ec);
		const bool inside = !ec && !relative.empty() && *relative.begin() != "..";

		reg.emplace<ModelSourceComponent>(model, inside ? relative.generic_string() : std::string(location),
										  MaterialWorkflow::PHONG);

		AddChild(root, model);

		return model;
	}

	entt::entity Scene::CreatePointLight(const glm::vec3 pos, const glm::vec3 color)
	{
		const entt::entity light =
			CreateEntity("Point light " + std::to_string(std::rand()), root);

		reg.get<TransformComponent>(light).Position = pos;
		reg.emplace<PointLightComponent>(light, color);

		return light;
	}
}
