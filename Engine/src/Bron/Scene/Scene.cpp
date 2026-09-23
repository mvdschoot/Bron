#include "Scene.h"

#include <algorithm>

#include "Bron/Scene/AssetManager.h"
#include "Bron/Scripting/LuaManager.h"
#include "Bron/Core/Timestep.h"

#include <set>

namespace bron {
Scene::Scene() : light_management(*this), lua_manager(CreateScope<lua::LuaManager>(this)) {
	root = CreateEntity("Root node");
}

Scene::~Scene() = default;

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

entt::entity Scene::Instantiate(const assets::AssetHandle& model) {
	const Ref<assets::ModelAsset> asset = assets::AssetManager::Instance().Get<assets::ModelAsset>(model);
	if (!asset || asset->nodes.empty())
		return entt::null;

	// Nodes come parent first, so each one's parent entity already exists.
	std::vector<entt::entity> created(asset->nodes.size(), static_cast<entt::entity>(entt::null));
	for (usize i = 0; i < asset->nodes.size(); i++) {
		const assets::ModelAsset::Node& node = asset->nodes[i];

		const entt::entity parent = node.parent.has_value() ? created[*node.parent] : entt::null;
		const entt::entity entity = CreateEntity(node.name, parent);
		created[i] = entity;

		TransformComponent& transform = reg.get<TransformComponent>(entity);
		transform.Position = node.position;
		transform.RotationQuat = node.rotation;
		transform.Scaling = node.scale;

		if (node.meshes.size() == 1) {
			reg.emplace<MeshMaterialComponent>(entity, node.meshes[0].mesh, node.meshes[0].material);
			continue;
		}

		// A mesh has no transform of its own, so each child sits exactly on the node.
		for (const assets::ModelAsset::Submesh& submesh: node.meshes) {
			const entt::entity child = CreateEntity(submesh.name, entity);
			reg.emplace<MeshMaterialComponent>(child, submesh.mesh, submesh.material);
		}
	}

	return created[0];
}

entt::entity Scene::CreateModel(const std::filesystem::path& path, const MaterialWorkflow workflow) {
	const std::optional<assets::AssetHandle> model = assets::AssetManager::Instance().LoadModel(path, workflow);
	return model.has_value() ? Instantiate(*model) : entt::null;
}


entt::entity Scene::PrimaryCamera() const {
	entt::entity fallback = entt::null;

	for (const auto [entity, camera]: reg.view<const CameraComponent>().each()) {
		if (camera.primary)
			return entity;

		if (fallback == entt::null)
			fallback = entity;
	}

	if (fallback != entt::null)
		BR_CORE_WARN("No camera in this scene is marked primary; using the first one found.");

	return fallback;
}

void Scene::OnRuntimeStart() {
	for (auto [entity, script]: reg.view<ScriptComponent>().each()) {
		for (std::filesystem::path& location: script.scripts) {
			lua_manager->AttachScript(location, entity);
		}
	}

	lua_manager->OnStart();

	BR_CORE_INFO("Loaded & started scripts");
}

void Scene::OnUpdate(Timestep ts) const { lua_manager->OnUpdate(ts); }

void Scene::OnEvent(Event& event) const { lua_manager->OnEvent(event); }
} // namespace bron
