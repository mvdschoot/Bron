#pragma once

#include "Bron.h"

namespace Bron::Editor
{
	/// The camera the editor viewport looks through: an orbit ("turntable") camera that
	/// always points at a focus point and is moved by changing the two angles and the
	/// distance around it, never by writing a position directly.
	class EditorCamera final : public FrustumCamera
	{
	public:
		EditorCamera(float fovY, float aspectRatio, float nearPlane, float farPlane);

		/// Applies the held-key orbit controls. Called once per frame.
		void OnUpdate(Timestep ts);

		/// Zooms towards or away from the focus point.
		bool OnMouseScrolled(MouseScrolledEvent& e);

		/// Re-centres the orbit on a point in the world, looking at it from a fixed angle.
		void Focus(const glm::vec3& point);

	private:
		/// Derives the world position from the focus point and the orbit parameters.
		void UpdatePosition();

		glm::vec3 mFocus{0.0f};

		float mAzimuth = 0.0f;     // Rotation in the XZ plane, radians.
		float mElevation = 0.5f;   // Tilt above the XZ plane, radians, clamped to +-PI/2.
		float mDistance = 10.0f;   // Distance from the focus point.
	};
}
