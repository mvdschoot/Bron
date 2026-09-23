#include "Bron/Scene/AssetDependencies.h"

#include <set>

#include "Bron/Scene/AssetManager.h"

namespace bron {
std::vector<std::filesystem::path> CollectAssets(const Scene& scene) {
	// Ordered and deduplicated: two entities sharing a model is the normal case, and an
	// exporter copying the same file twice is at best wasted work.
	std::set<std::filesystem::path> found;

	assets::AssetManager& manager = assets::AssetManager::Instance();

	auto add = [&](const assets::AssetHandle& handle) {
		const std::optional<std::filesystem::path> file = manager.SourceFile(handle);
		if (!file.has_value())
			return;

		std::filesystem::path meta = *file;
		meta += ".meta";

		found.insert(file->lexically_normal());
		found.insert(meta.lexically_normal());
	};

	for (const auto [entity, mesh]: scene.reg.view<const MeshMaterialComponent>().each()) {
		add(mesh.mesh);
		add(mesh.material);

		// Textures on disk are assets of their own rather than part of the model, so they
		// only show up by asking the material.
		if (const Ref<assets::MaterialAsset> material = manager.Get<assets::MaterialAsset>(mesh.material)) {
			for (const auto& [type, texture]: material->textures)
				add(texture);
		}
	}

	// Every other component that comes to refer to an asset belongs here, next to this
	// one. A ScriptComponent is the next of them.

	return {found.begin(), found.end()};
}
} // namespace bron
