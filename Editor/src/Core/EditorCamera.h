#pragma once

#include "Bron.h"

namespace bron::editor {
/// The camera the editor viewport looks through: an Orbit("turntable") camera that
/// always points at a focus point and is moved by changing the two angles and the
/// distance around it, never by writing a position directly.
class EditorCamera final : public FrustumCamera {
public:
	EditorCamera(float fov_y, float aspect_ratio, float near_plane, float far_plane);

	/// Applies the held-key orbit controls. Called once per frame.
	void OnUpdate(Timestep ts);

	/// Zooms towards or away from the focus point.
	bool OnMouseScrolled(MouseScrolledEvent& e);

	/// Re-centres the orbit on a point in the world, looking at it from a fixed angle.
	void Focus(const glm::vec3& point);

private:
	/// Derives the world position from the focus point and the orbit parameters.
	void UpdatePosition();

	glm::vec3 focus_{0.0f};

	float azimuth_ = 0.0f; // Rotation in the XZ plane, radians.
	float elevation_ = 0.5f; // Tilt above the XZ plane, radians, clamped to +-kPi/2.
	float distance_ = 10.0f; // Distance from the focus point.
};
} // namespace bron::editor
