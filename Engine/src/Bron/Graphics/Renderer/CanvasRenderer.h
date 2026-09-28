#pragma once

#include "Bron/Scene/Scene.h"

#include <glm/glm.hpp>

namespace bron {
struct CameraView;

/// Draws a scene's canvases: every Box2DComponent and Text2DComponent, laid out by the
/// RectTransformComponents above it. The scene-facing half of 2D; R2D is the drawing half.
///
/// Each canvas is walked depth first, so a parent is drawn before its children and a
/// later sibling over an earlier one. That is the whole of 2D draw order: there is no
/// depth test to fall back on.
class CanvasRenderer {
public:
	/// World-space canvases, placed by their Transform and seen through 'view'. For now
	/// they are drawn over the 3D scene rather than depth tested against it.
	static void DrawWorld(Scene& scene, const CameraView& view);

	/// Screen-space canvases, in order of their sort_order. Each is scaled so its
	/// reference height fills the target, and is as wide as the target's aspect makes it.
	static void DrawScreen(Scene& scene, glm::vec2 target_size);
};
} // namespace bron
