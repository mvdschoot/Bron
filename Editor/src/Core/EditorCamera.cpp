#include "Core/EditorCamera.h"

#include "Core/Preferences.h"

namespace Bron::Editor
{
	namespace
	{
		// How close the camera may get to the focus point before zooming stops.
		constexpr float MinDistance = 0.15f;
	}

	EditorCamera::EditorCamera(const float fovY, const float aspectRatio, const float nearPlane, const float farPlane)
		: FrustumCamera(fovY, aspectRatio, nearPlane, farPlane,
						glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{0.0f, 1.0f, 0.0f})
	{
		UpdatePosition();
	}

	void EditorCamera::OnUpdate(const Timestep ts)
	{
		const float dt = ts.getSeconds() * Preferences::Get().cameraOrbitSpeed;

		if (Input::isKeyPressed(Key::A))
			mAzimuth += dt;
		if (Input::isKeyPressed(Key::D))
			mAzimuth -= dt;

		// Stopping just short of the poles keeps the up vector meaningful.
		if (Input::isKeyPressed(Key::W))
			mElevation += (mElevation > 0.5f * PI ? 0.0f : dt);
		if (Input::isKeyPressed(Key::S))
			mElevation -= (mElevation < -0.5f * PI ? 0.0f : dt);

		UpdatePosition();
	}

	bool EditorCamera::OnMouseScrolled(MouseScrolledEvent& e)
	{
		mDistance = std::max(mDistance - e.getOffsetY() * Preferences::Get().cameraZoomSpeed, MinDistance);
		UpdatePosition();
		return true;
	}

	void EditorCamera::Focus(const glm::vec3& point)
	{
		mFocus = point;
		mAzimuth = 0.0f;
		mElevation = 0.5f;

		SetTarget(mFocus);
		UpdatePosition();
	}

	void EditorCamera::UpdatePosition()
	{
		SetPosition({
			mFocus.x + cos(mAzimuth) * cos(mElevation) * mDistance,
			mFocus.y + sin(mElevation) * mDistance,
			mFocus.z + sin(mAzimuth) * cos(mElevation) * mDistance
		});
	}
}
