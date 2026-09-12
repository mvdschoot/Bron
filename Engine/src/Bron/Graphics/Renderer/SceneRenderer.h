#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Scene/Scene.h"

namespace bron {
struct RenderStatistics {
	u32 DrawCalls;
	u32 Shaders;
	u32 Materials;
	u32 Meshes;
	u32 UniformCalls;
};

class SceneRenderer {
public:
	static void Init();
	static void Draw(Scene& scene);

	/// Draws a solid outline around the given meshes, in two passes over the same
	/// geometry: one to mark where they are, one to draw an enlarged copy everywhere they
	/// are not. 'width' is in world units. Expects the scene pass to have run first - it
	/// reads the depth buffer that pass left behind.
	static void DrawOutline(Scene& scene, const std::vector<entt::entity>& meshes,
							glm::vec3 color = {0.8f, 0.5f, 0.1f}, float width = 0.05f);

	static RenderStatistics Statistics;
};
} // namespace bron
