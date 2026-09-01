#include "FrustumCamera.h"

namespace Steve
{
	FrustumCamera::FrustumCamera(float fovy, float aspect_ratio, float near, float far, glm::vec3 position, glm::vec3 target, glm::vec3 up)
		: mProjectionMat(glm::perspective(fovy, aspect_ratio, near, far)), mFovY(fovy), mAspectRatio(aspect_ratio), mNear(near), mFar(far), mPosition(position), mTarget(target), mUp(up)
	{
	}

	void FrustumCamera::SetAspectRatio(const float aspect_ratio)
	{
		if (aspect_ratio <= 0.0f || aspect_ratio == mAspectRatio)
			return;

		mAspectRatio = aspect_ratio;
		mProjectionMat = glm::perspective(mFovY, mAspectRatio, mNear, mFar);
	}

	void FrustumCamera::SetPosition(glm::vec3 pos)
	{
		mPosition = pos;
	}

	void FrustumCamera::SetTarget(glm::vec3 target)
	{
		mTarget = target;
	}

	void FrustumCamera::SetUpvector(glm::vec3 up)
	{
		mUp = up;
	}
}

