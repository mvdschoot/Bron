//
// Created by mathi on 3-9-2026.
//

#include "Serialization.h"

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "Bron/Core/Logger.h"
#include "Bron/Util/Paths.h"

namespace bron {
namespace {
// Bumped whenever the layout below changes in a way older files cannot satisfy.
//
// 2: meshes are saved as entities holding asset handles. Version 1 saved only the path of
//    each model and re-imported it on load; those files are still read, see
//    InstantiateLegacyModels.
constexpr int kSceneVersion = 2;
constexpr int kOldestReadableVersion = 1;

// Depth first, so siblings land in the file in their current order and can
// simply be re-parented in that order on the way back in.
void WriteEntity(const Scene& scene, const entt::entity entity, nlohmann::json& out) {
	const entt::registry& reg = scene.reg;
	const HierarchyComponent& hierarchy = reg.get<HierarchyComponent>(entity);

	nlohmann::json entry;
	entry["id"] = reg.get<IDComponent>(entity).id;
	entry["name"] = reg.get<TagComponent>(entity).name;
	entry["transform"] = reg.get<TransformComponent>(entity);
	entry["visible"] = reg.get<VisibilityComponent>(entity).visible;
	entry["parent"] = hierarchy.parent == entt::null ? nlohmann::json(nullptr)
													 : nlohmann::json(reg.get<IDComponent>(hierarchy.parent).id);

	if (const PointLightComponent* light = reg.try_get<PointLightComponent>(entity)) {
		entry["pointLight"] = *light;
	}

	if (const MeshMaterialComponent* mesh = reg.try_get<MeshMaterialComponent>(entity)) {
		entry["mesh"] = *mesh;
	}

	if (const CameraComponent* camera = reg.try_get<CameraComponent>(entity)) {
		entry["camera"] = *camera;
	}

	if (const ScriptComponent* script = reg.try_get<ScriptComponent>(entity)) {
		entry["script"] = *script;
	}

	out.push_back(std::move(entry));

	for (const entt::entity child: hierarchy.children) {
		WriteEntity(scene, child, out);
	}
}

// Version 1 scenes saved a model as its root entity plus {"model": {"path", "workflow"}}
// and left the meshes out. The model is placed again here and its entities are moved under
// the root that was read from the file, which keeps the transform the user gave it. The
// next save writes the meshes out like any other entity.
void InstantiateLegacyModels(Scene& scene, const nlohmann::json& entities, const std::vector<entt::entity>& created) {
	for (std::size_t i = 0; i < created.size(); ++i) {
		if (!entities[i].contains("model"))
			continue;

		const nlohmann::json& model = entities[i].at("model");
		const std::filesystem::path path = paths::ResolveAsset(model.at("path").get<std::string>());
		const MaterialWorkflow workflow = model.value("workflow", kPhong);

		const entt::entity imported = scene.CreateModel(path, workflow);
		if (imported == entt::null) {
			BR_CORE_WARN("Could not place model {} again", path.string());
			continue;
		}

		// The imported root carries the model file's own root transform and meshes; the
		// saved entity takes its place, so both have to be moved across.
		if (const MeshMaterialComponent* mesh = scene.reg.try_get<MeshMaterialComponent>(imported))
			scene.reg.emplace<MeshMaterialComponent>(created[i], *mesh);

		// Copied, because AddChild mutates the vector it is read from.
		const std::vector<entt::entity> children = scene.reg.get<HierarchyComponent>(imported).children;
		for (const entt::entity child: children)
			scene.AddChild(created[i], child);

		scene.DestroyEntity(imported);
	}
}
} // namespace

void Serialization::SerializeScene(const Scene& scene, const std::filesystem::path& file) {
	nlohmann::json entities = nlohmann::json::array();

	// Only what hangs off the root is part of the scene; anything unparented is
	// scratch and is not written.
	if (scene.root != entt::null) {
		WriteEntity(scene, scene.root, entities);
	}

	nlohmann::json document;
	document["version"] = kSceneVersion;
	document["root"] = scene.root == entt::null ? nlohmann::json(nullptr)
												: nlohmann::json(scene.reg.get<IDComponent>(scene.root).id);
	document["entities"] = std::move(entities);

	std::ofstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not open {} for writing", file.string());
		return;
	}

	stream << document.dump(1, '	');
	BR_CORE_INFO("Saved scene to {}", file.string());
}

void Serialization::DeserializeScene(Scene& scene, const std::filesystem::path& file) {
	std::ifstream stream(file);
	if (!stream) {
		BR_CORE_ERROR("Could not open {} for reading", file.string());
		return;
	}

	const nlohmann::json document = nlohmann::json::parse(stream, nullptr, false);
	if (document.is_discarded()) {
		BR_CORE_ERROR("{} is not valid json", file.string());
		return;
	}

	const int version = document.value("version", 0);
	if (version < kOldestReadableVersion || version > kSceneVersion) {
		BR_CORE_ERROR("{} is a version {} scene, this build reads versions {} to {}", file.string(), version,
					  kOldestReadableVersion, kSceneVersion);
		return;
	}

	const nlohmann::json& entities = document.at("entities");

	scene.reg.clear();
	scene.root = entt::null;

	std::unordered_map<std::string, entt::entity> by_id;
	std::vector<entt::entity> created;
	created.reserve(entities.size());

	// Every entity has to exist before any parent can be resolved, so the
	// hierarchy is wired up in a second pass.
	for (const nlohmann::json& entry: entities) {
		const entt::entity entity = scene.reg.create();

		scene.reg.emplace<IDComponent>(entity, entry.at("id").get<UUID>());
		scene.reg.emplace<TagComponent>(entity, entry.at("name").get<std::string>());
		scene.reg.emplace<TransformComponent>(entity, entry.at("transform").get<TransformComponent>());
		scene.reg.emplace<HierarchyComponent>(entity);
		scene.reg.emplace<VisibilityComponent>(entity, entry.value("visible", true));

		if (entry.contains("pointLight")) {
			scene.reg.emplace<PointLightComponent>(entity, entry.at("pointLight").get<PointLightComponent>());
		}

		if (entry.contains("camera")) {
			scene.reg.emplace<CameraComponent>(entity, entry.at("camera").get<CameraComponent>());
		}

		if (entry.contains("mesh")) {
			scene.reg.emplace<MeshMaterialComponent>(entity, entry.at("mesh").get<MeshMaterialComponent>());
		}

		if (entry.contains("script")) {
			scene.reg.emplace<ScriptComponent>(entity, entry.at("script").get<ScriptComponent>());
		}

		by_id.emplace(entry.at("id").get<std::string>(), entity);
		created.push_back(entity);
	}

	for (std::size_t i = 0; i < created.size(); ++i) {
		const nlohmann::json& parent = entities[i].at("parent");
		if (parent.is_null()) {
			continue;
		}

		const auto it = by_id.find(parent.get<std::string>());
		if (it == by_id.end()) {
			BR_CORE_WARN("Entity {} names a parent that is not in the file", entities[i].value("name", ""));
			continue;
		}

		scene.AddChild(it->second, created[i]);
	}

	if (version == 1)
		InstantiateLegacyModels(scene, entities, created);

	if (const nlohmann::json& root = document.at("root"); !root.is_null()) {
		const auto it = by_id.find(root.get<std::string>());
		if (it != by_id.end()) {
			scene.root = it->second;
		}
	}

	if (scene.root == entt::null) {
		BR_CORE_WARN("{} has no root entity; the scene has no place to parent new entities", file.string());
	}

	BR_CORE_INFO("Loaded scene from {}", file.string());
}
} // namespace bron
