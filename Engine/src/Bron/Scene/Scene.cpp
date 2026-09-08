#include "Scene.h"

#include <algorithm>
#include <cstdlib>

#include "Bron/Graphics/Components/ModelLoader.h"
#include "Bron/Util/Paths.h"

namespace bron {
Scene::Scene() : light_management(*this) { root = CreateEntity("Root node"); }

entt::entity Scene::CreateEntity(const std::string& name, const entt::entity parent) {
	const entt::entity entity = reg.create();

	reg.emplace<IDComponent>(entity);
	reg.emplace<TagComponent>(entity, name);
	reg.emplace<TransformComponent>(entity);
	reg.emplace<HierarchyComponent>(entity);
	reg.emplace<VisibilityComponent>(entity);

	if (parent != entt::null) {
		AddChild(parent, entity);
	}

	return entity;
}

void Scene::DestroyEntity(const entt::entity entity) {
	BR_CORE_ASSERT(reg.valid(entity), "Cannot destroy an entity that does not exist");

	HierarchyComponent& hierarchy = reg.get<HierarchyComponent>(entity);

	// Detach from the parent first, so the recursion below cannot walk back up into it.
	if (hierarchy.parent != entt::null) {
		RemoveChild(hierarchy.parent, entity);
	}

	// Copy, because destroying a child mutates the vector we would otherwise be iterating.
	const std::vector<entt::entity> children = hierarchy.children;
	for (const entt::entity child: children) {
		DestroyEntity(child);
	}

	reg.destroy(entity);
}

void Scene::AddChild(const entt::entity parent, const entt::entity child) {
	BR_CORE_ASSERT(reg.valid(parent) && reg.valid(child), "Cannot parent an entity that does not exist");
	BR_CORE_ASSERT(parent != child, "An entity cannot be its own parent");

	HierarchyComponent& child_hierarchy = reg.get<HierarchyComponent>(child);

	// Re-parenting: unhook from the previous parent first.
	if (child_hierarchy.parent != entt::null) {
		RemoveChild(child_hierarchy.parent, child);
	}

	child_hierarchy.parent = parent;
	reg.get<HierarchyComponent>(parent).children.push_back(child);
}

void Scene::RemoveChild(const entt::entity parent, const entt::entity child) {
	std::vector<entt::entity>& children = reg.get<HierarchyComponent>(parent).children;

	const auto it = std::find(children.begin(), children.end(), child);
	BR_CORE_ASSERT(it != children.end(), "Entity is not a child of this parent");

	children.erase(it);
	reg.get<HierarchyComponent>(child).parent = entt::null;
}

glm::mat4 Scene::WorldTransform(const entt::entity entity) {
	BR_PROFILE_FUNCTION();

	glm::mat4 transform = *reg.get<TransformComponent>(entity);

	entt::entity parent = reg.get<HierarchyComponent>(entity).parent;
	while (parent != entt::null) {
		transform = *reg.get<TransformComponent>(parent) * transform;
		parent = reg.get<HierarchyComponent>(parent).parent;
	}

	return transform;
}
bool Scene::IsVisible(entt::entity entity) {

	bool is_visible = reg.get<VisibilityComponent>(entity).visible;

	entt::entity parent = reg.get<HierarchyComponent>(entity).parent;
	while (parent != entt::null) {
		if (!is_visible) {
			return false;
		}

		is_visible &= reg.get<VisibilityComponent>(parent).visible;
		parent = reg.get<HierarchyComponent>(parent).parent;
	}

	return is_visible;
}

entt::entity Scene::CreatePhongModel(const std::filesystem::path& path) {
	entt::entity model_entity = CreateEntity(path.stem().string());
	ModelLoader::LoadModel(*this, MaterialWorkflow::kPhong, path);

	// Recorded relative to the asset root so a save file survives the project
	// being moved. paths::RelativeToAsset keeps a location outside the root as it is;
	// joining an absolute path back onto the root is a no-op, so loading still works.
	reg.emplace<ModelSourceComponent>(model_entity, paths::RelativeToAsset(path).generic_string(), MaterialWorkflow::kPhong);

	return model_entity;
}

} // namespace bron
