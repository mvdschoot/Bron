#pragma once

#include "Bron/Core/Core.h"

#include "Bron/Graphics/CameraView.h"
#include "Bron/Scene/Scene.h"

#include <glm/glm.hpp>

namespace bron {
/// Renders a whole scene into the bound target: the world through 'view' first, then its
/// screen-space 2D elements on top, in pixels over 'target_size'.
///
/// Draws the scene and nothing else. Whatever an application wants to add - the editor's
/// grid, outlines and gizmos - goes between DrawWorld and DrawScreen, so it sits over the
/// world and under the HUD.
class SceneRenderer {
public:
	static void Init();

	/// Both passes, for an application that adds nothing in between.
	static void Draw(Scene& scene, const CameraView& view, glm::vec2 target_size);

	/// Meshes, then world-space canvases.
	static void DrawWorld(Scene& scene, const CameraView& view);

	/// Screen-space canvases.
	static void DrawScreen(Scene& scene, glm::vec2 target_size);
};
} // namespace bron
