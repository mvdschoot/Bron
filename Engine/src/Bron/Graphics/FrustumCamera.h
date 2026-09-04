#pragma once

#include "Bron/Core/Core.h"

#include "Bron/Graphics/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace bron {
class FrustumCamera : public Camera {
public:
	FrustumCamera(float fovy, float aspect_ratio, float near, float far, glm::vec3 position, glm::vec3 target,
				  glm::vec3 up);


	[[nodiscard]] glm::mat4 GetProjectionMatrix() const override { return projection_mat_; }
	[[nodiscard]] glm::mat4 GetViewMatrix() const override { return glm::lookAt(position_, target_, up_); }
	[[nodiscard]] glm::mat4 GetVPmatrix() const override { return projection_mat_ * GetViewMatrix(); }
	[[nodiscard]] glm::vec3 GetPosition() const override { return position_; }
	[[nodiscard]] glm::vec3 GetDirection() const override { return target_; }

	/// Keeps the projection matching the render target, so a resize widens the view
	/// instead of stretching what was already on screen.
	void SetAspectRatio(float aspect_ratio);

	void SetPosition(glm::vec3 pos);
	void SetTarget(glm::vec3 target);
	void SetUpvector(glm::vec3 up);

private:
	glm::mat4 projection_mat_;

	float fov_y_;
	float aspect_ratio_;
	float near_;
	float far_;

	glm::vec3 position_;
	glm::vec3 target_;
	glm::vec3 up_;
};
} // namespace bron
