#pragma once

#include "Bron/Core/Core.h"

#include "Bron/Graphics/CameraView.h"
#include "Bron/Scene/Scene.h"

#include <glm/glm.hpp>

#include <vector>

namespace bron {
/// Renders a whole scene into the bound target: the world through 'view' first, then its
/// screen-space 2D elements on top, in pixels over 'target_size'.
class SceneRenderer {
public:
	static void Init();

	/// 'outlined' meshes get a selection outline, drawn after the world and before the
	/// 2D pass so it never covers the HUD.
	static void Draw(Scene& scene, const CameraView& view, glm::vec2 target_size,
					 const std::vector<entt::entity>& outlined = {});
};
} // namespace bron
