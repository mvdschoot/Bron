#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "../Camera.h"

#include "glm/glm.hpp"

namespace bron {
/// How the editor grid looks. These are the same knobs a scene editor puts in its
/// settings panel: how big one cell is, how often a line is a "primary" one, and how far
/// the grid reaches before it fades away.
struct GridSettings {
	/// World units across one cell at the finest level. Every other size is derived from
	/// it, so this is the only place the grid is tied to the scale of the world.
	float cell_size = 1.0f;

	/// Every Nth line is drawn as a primary line, and the grid coarsens by this same
	/// factor each time it steps down a level of detail. Godot calls it "primary grid
	/// steps"; a value of 10 gives the familiar metric look of 1m cells with a heavier
	/// line every 10m.
	float primary_every = 10.0f;

	/// How far the grid reaches, as a multiple of the camera's height above the plane.
	/// It is relative rather than absolute on purpose: tie the reach to the camera and
	/// the grid looks the same whether you are 2 or 200 units up, which is what makes it
	/// read as infinite.
	float view_distance = 25.0f;

	glm::vec3 line_color{0.35f, 0.35f, 0.35f};
	glm::vec3 primary_line_color{0.6f, 0.6f, 0.6f};

	/// Axis colours follow the usual convention: X is red, Z is blue.
	glm::vec3 x_axis_color{0.85f, 0.25f, 0.3f};
	glm::vec3 z_axis_color{0.25f, 0.45f, 0.85f};
	bool draw_axes = true;
};

/// The infinite ground grid on the world XZ plane.
///
/// No grid geometry exists. A single screen-filling quad is drawn, and every fragment
/// intersects its own view ray with the y = 0 plane to find which point of the floor it
/// is looking at - so the grid is exact at any distance and never runs out of cells. The
/// pattern, the level of detail and the distance fade are all decided per pixel from that
/// world position; see Grid.cpp and the kGrid shader.
///
/// Draw() expects blending to be enabled and does not change render state. It writes
/// gl_FragDepth from the point it hit, so the grid depth-tests against scene geometry
/// like an ordinary surface.
class GridRenderer {
public:
	static void Init(Camera* camera);
	static void Draw();

	/// Mutable so the editor can drive the grid from a settings panel; changes apply on
	/// the next Draw().
	static GridSettings& Settings();
};
} // namespace bron
