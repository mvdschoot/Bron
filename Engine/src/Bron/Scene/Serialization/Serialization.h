#pragma once

#include <filesystem>

#include "../Scene.h"

namespace Bron {

	// The scene is written as a flat list of entities keyed by UUID rather than
	// as an entt snapshot. entt::entity values are positions in a registry, so a
	// snapshot only means anything in an empty registry - it can never be merged
	// into a live scene or instantiated twice as a prefab. Identifiers also let
	// entities that a loader generates (the meshes under a model) be left out of
	// the file and rebuilt from their source path on load.
	class Serialization {
	public:
		static void SerializeScene(const Scene &scene, const std::filesystem::path &file);

		// Replaces everything in the scene. Handles into the old scene - Scene::root,
		// the editor's selection - are invalid afterwards; root is repointed here,
		// the rest is the caller's problem.
		static void DeserializeScene(Scene &scene, const std::filesystem::path &file);
	};

} // namespace Bron
