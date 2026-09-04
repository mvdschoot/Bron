#pragma once

#include "Bron/Core/Core.h"

#include "Bron/Graphics/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace bron
{
	class BR_API OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio);
		OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio, glm::vec3 position,
		                   float rotation);


		[[nodiscard]] glm::mat4 GetProjectionMatrix() const override { return projection_mat_; }
		[[nodiscard]] glm::mat4 GetViewMatrix() const override { return view_mat_; }
		[[nodiscard]] glm::mat4 GetVPmatrix() const override { return projection_mat_ * view_mat_; }
		[[nodiscard]] glm::vec3 GetPosition() const override { return position_; }
		[[nodiscard]] glm::vec3 GetDirection() const override { return { 0.0, 0.0, 0.0 }; }

		void SetProjection(float left, float right, float top, float bottom);

		void SetPosition(glm::vec3 pos);
		void SetRotation(float angle);
		void SetZoom(float zoom);


		float AspectRatio;
	private:
		void Recalculate();

		glm::mat4 projection_mat_;
		glm::mat4 view_mat_;

		glm::vec3 position_;
		glm::vec3 target_;
		float rotation_;
		float zoom_level_;
	};
}
