#include "Core/EditorCamera.h"

#include "Core/Preferences.h"

#include <glm/gtc/matrix_transform.hpp>

namespace bron::editor {
namespace {
// How close the camera may get to the focus point before zooming stops.
constexpr float kMinDistance = 0.15f;

// The orbit never rolls, so up is always world up.
constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
} // namespace

EditorCamera::EditorCamera(const float fov_y, const float near_plane, const float far_plane) :
	fov_y_(fov_y), near_plane_(near_plane), far_plane_(far_plane) {}

void EditorCamera::OnUpdate(const Timestep ts) {
	const float dt = ts.GetSeconds() * Preferences::Get().camera_orbit_speed;

	if (Input::IsKeyPressed(key::A))
		azimuth_ += dt;
	if (Input::IsKeyPressed(key::D))
		azimuth_ -= dt;

	// Stopping just short of the poles keeps the up vector meaningful.
	if (Input::IsKeyPressed(key::W))
		elevation_ += (elevation_ > 0.5f * kPi ? 0.0f : dt);
	if (Input::IsKeyPressed(key::S))
		elevation_ -= (elevation_ < -0.5f * kPi ? 0.0f : dt);
}

bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e) {
	distance_ = std::max(distance_ - e.GetOffsetY() * Preferences::Get().camera_zoom_speed, kMinDistance);
	return true;
}

void EditorCamera::Focus(const glm::vec3& point) {
	focus_ = point;
	azimuth_ = 0.0f;
	elevation_ = 0.5f;
}

glm::vec3 EditorCamera::Position() const {
	return {focus_.x + cos(azimuth_) * cos(elevation_) * distance_, focus_.y + sin(elevation_) * distance_,
			focus_.z + sin(azimuth_) * cos(elevation_) * distance_};
}

CameraView EditorCamera::View(const float aspect) const {
	const glm::vec3 position = Position();

	CameraView view;
	// A collapsed panel would give an aspect of 0 or worse; glm::perspective divides by
	// it, so the last sane shape is kept until there is something to draw into again.
	view.projection = glm::perspective(fov_y_, aspect > 0.0f ? aspect : 1.0f, near_plane_, far_plane_);
	view.view = glm::lookAt(position, focus_, kUp);
	view.position = position;
	return view;
}
} // namespace bron::editor
