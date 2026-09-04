#include "Core/EditorCamera.h"

#include "Core/Preferences.h"

namespace bron::editor {
namespace {
// How close the camera may get to the focus point before zooming stops.
constexpr float kMinDistance = 0.15f;
} // namespace

EditorCamera::EditorCamera(const float fov_y, const float aspect_ratio, const float near_plane, const float far_plane) :
	FrustumCamera(fov_y, aspect_ratio, near_plane, far_plane, glm::vec3{0.0f}, glm::vec3{0.0f},
				  glm::vec3{0.0f, 1.0f, 0.0f}) {
	UpdatePosition();
}

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

	UpdatePosition();
}

bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e) {
	distance_ = std::max(distance_ - e.GetOffsetY() * Preferences::Get().camera_zoom_speed, kMinDistance);
	UpdatePosition();
	return true;
}

void EditorCamera::Focus(const glm::vec3& point) {
	focus_ = point;
	azimuth_ = 0.0f;
	elevation_ = 0.5f;

	SetTarget(focus_);
	UpdatePosition();
}

void EditorCamera::UpdatePosition() {
	SetPosition({focus_.x + cos(azimuth_) * cos(elevation_) * distance_, focus_.y + sin(elevation_) * distance_,
				 focus_.z + sin(azimuth_) * cos(elevation_) * distance_});
}
} // namespace bron::editor
