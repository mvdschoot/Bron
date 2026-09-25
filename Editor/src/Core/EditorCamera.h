#pragma once

#include "Bron.h"

namespace bron::editor {
class PanelInput;
/// The camera the editor viewport looks through: an orbit ("turntable") camera that
/// always points at a focus point and is moved by changing the two angles and the
/// distance around it, never by writing a position directly.
///
/// It is a controller, not something the renderer knows about. It holds the state a
/// person edits and hands out a CameraView on request; nothing downstream sees this
/// class. It is also editor-only - a scene's own cameras are entities carrying a
/// CameraComponent, and are not this.
class EditorCamera final {
public:
	EditorCamera(float fov_y, float near_plane, float far_plane);

	/// Applies the held-key orbit controls. Called once per frame.
	void OnUpdate(const PanelInput& input, Timestep ts);

	/// Zooms towards or away from the focus point.
	bool OnMouseScrolled(MouseScrolledEvent& e);

	/// Re-centres the orbit on a point in the world, looking at it from a fixed angle.
	void Focus(const glm::vec3& point);

	/// What the viewport is drawn through. 'aspect' is the render target's, which is why
	/// it is an argument: the camera has no idea how big the panel showing it is, and the
	/// same camera would give a different projection in a differently shaped one.
	[[nodiscard]] CameraView View(float aspect) const;

	/// Derived from the focus point and the orbit parameters; there is nowhere else the
	/// position is kept.
	[[nodiscard]] glm::vec3 Position() const;

private:
	float fov_y_; // Vertical, radians.
	float near_plane_;
	float far_plane_;

	glm::vec3 focus_{0.0f};

	float azimuth_ = 0.0f; // Rotation in the XZ plane, radians.
	float elevation_ = 0.5f; // Tilt above the XZ plane, radians, clamped to +-kPi/2.
	float distance_ = 10.0f; // Distance from the focus point.
};
} // namespace bron::editor
