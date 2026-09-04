#include "FrustumCamera.h"

namespace bron
{
	FrustumCamera::FrustumCamera(float fovy, float aspect_ratio, float near, float far, glm::vec3 position, glm::vec3 target, glm::vec3 up)
		: projection_mat_(glm::perspective(fovy, aspect_ratio, near, far)), fov_y_(fovy), aspect_ratio_(aspect_ratio), near_(near), far_(far), position_(position), target_(target), up_(up)
	{
	}

	void FrustumCamera::SetAspectRatio(const float aspect_ratio)
	{
		if (aspect_ratio <= 0.0f || aspect_ratio == aspect_ratio_)
			return;

		aspect_ratio_ = aspect_ratio;
		projection_mat_ = glm::perspective(fov_y_, aspect_ratio_, near_, far_);
	}

	void FrustumCamera::SetPosition(glm::vec3 pos)
	{
		position_ = pos;
	}

	void FrustumCamera::SetTarget(glm::vec3 target)
	{
		target_ = target;
	}

	void FrustumCamera::SetUpvector(glm::vec3 up)
	{
		up_ = up;
	}
}

