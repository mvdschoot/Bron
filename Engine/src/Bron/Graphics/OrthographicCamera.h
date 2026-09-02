#pragma once

#include "Bron/Core/Core.h"

#include "Bron/Graphics/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Bron
{
	class BR_API OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio);
		OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio, glm::vec3 position,
		                   float rotation);


		[[nodiscard]] glm::mat4 GetProjectionMatrix() const override { return mProjectionMat; }
		[[nodiscard]] glm::mat4 GetViewMatrix() const override { return mViewMat; }
		[[nodiscard]] glm::mat4 GetVPmatrix() const override { return mProjectionMat * mViewMat; }
		[[nodiscard]] glm::vec3 GetPosition() const override { return mPosition; }
		[[nodiscard]] glm::vec3 GetDirection() const override { return { 0.0, 0.0, 0.0 }; }

		void SetProjection(float left, float right, float top, float bottom);

		void SetPosition(glm::vec3 pos);
		void SetRotation(float angle);
		void SetZoom(float zoom);


		float AspectRatio;
	private:
		void recalculate();

		glm::mat4 mProjectionMat;
		glm::mat4 mViewMat;

		glm::vec3 mPosition;
		glm::vec3 mTarget;
		float mRotation;
		float mZoomLevel;
	};
}
