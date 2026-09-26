#pragma once

#include "Bron/Graphics/CameraView.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bron {
/// A flat camera for 2D drawing, in pixels: with the defaults, (0, 0) is the bottom-left
/// corner of the target and (width, height) the top-right. 'position' scrolls the view
/// and 'zoom' scales it, 2 meaning everything is drawn twice as large.
struct Camera2D {
	glm::vec2 position{0.0f};
	float zoom = 1.0f;

	[[nodiscard]] CameraView View(const glm::vec2 target_size) const {
		const glm::vec2 extent = target_size / zoom;

		CameraView view;
		view.projection = glm::ortho(0.0f, extent.x, 0.0f, extent.y, -1.0f, 1.0f);
		view.view = glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f));
		view.position = glm::vec3(position, 0.0f);
		return view;
	}
};
} // namespace bron
