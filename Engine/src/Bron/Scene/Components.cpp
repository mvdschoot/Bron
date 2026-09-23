#include "Components.h"

#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/Components/BufferExtentions.h"

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

namespace bron {
namespace {} // namespace

CameraView ViewFrom(const CameraComponent& camera, const glm::mat4& world_transform, const float aspect) {
	// A collapsed viewport gives an aspect of zero, and both projections divide by it.
	const float safe_aspect = aspect > 0.0f ? aspect : 1.0f;

	CameraView view;

	if (camera.projection == kPerspective) {
		view.projection = glm::perspective(camera.fov_y, safe_aspect, camera.near_plane, camera.far_plane);
	} else {
		// ortho_size is the full vertical extent, so the half-extents are half of it and
		// the width follows from the shape of the target.
		const float half_height = camera.ortho_size * 0.5f;
		const float half_width = half_height * safe_aspect;
		view.projection =
				glm::ortho(-half_width, half_width, -half_height, half_height, camera.near_plane, camera.far_plane);
	}

	// Inverting the world transform is only a view matrix while that transform is a
	// rotation and a translation. A scaled camera would scale everything seen through it,
	// which is never what was meant, so it is caught here rather than puzzled over later.
	BR_CORE_ASSERT(CompareFloat(glm::length(glm::vec3(world_transform[0])), 1.0f) &&
						   CompareFloat(glm::length(glm::vec3(world_transform[1])), 1.0f) &&
						   CompareFloat(glm::length(glm::vec3(world_transform[2])), 1.0f),
				   "A camera entity may not be scaled");

	view.view = glm::inverse(world_transform);
	view.position = glm::vec3(world_transform[3]);
	return view;
}
} // namespace bron
