#pragma once

#include "Bron/Core/Core.h"

#include <glm/glm.hpp>

namespace bron {
// What a shader needs in order to draw from a point of view, and nothing else.
//
// This is a value, not an object: it is computed fresh each frame and passed down to
// whatever is drawing, in the way a Timestep is. Nothing stores one, so there is never a
// second copy of a position to keep in step with the one that owns it.
//
// Where the numbers come from is the caller's business. The editor derives one from its
// orbit camera; the runtime derives one from the entity carrying the primary
// CameraComponent, whose pose already lives in its TransformComponent.
struct CameraView {
	glm::mat4 projection{1.0f};

	// The inverse of the camera's world transform. Built from a position and a rotation
	// only - a scale in the transform it came from would shear everything drawn through
	// it, so cameras are not scaled.
	glm::mat4 view{1.0f};

	// Where the camera is in world space. Carried alongside rather than extracted from
	// 'view' because lighting needs it every frame and the inverse is not free.
	glm::vec3 position{0.0f};

	[[nodiscard]] glm::mat4 ViewProjection() const { return projection * view; }
};
} // namespace bron
