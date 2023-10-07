#ifndef __FRUSTUMCAMERA_HEADER__
#define __FRUSTUMCAMERA_HEADER__

#include "Steve/Core/Core.h"

#include "Steve/Graphics/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Steve
{
	class FrustumCamera : public Camera
	{
	public:
		FrustumCamera(float fovy, float aspect_ratio, float near, float far, glm::vec3 position, glm::vec3 target, glm::vec3 up);


		[[nodiscard]] glm::mat4 GetProjectionMatrix() const override { return mProjectionMat; }
		[[nodiscard]] glm::mat4 GetViewMatrix() const override { return glm::lookAt(mPosition, mTarget, mUp); }
		[[nodiscard]] glm::mat4 GetVPmatrix() const override { return mProjectionMat * GetViewMatrix(); }
		[[nodiscard]] glm::vec3 GetPosition() const override { return mPosition; }
		[[nodiscard]] glm::vec3 GetDirection() const override { return mTarget; }

		void SetPosition(glm::vec3 pos);
		void SetTarget(glm::vec3 target);
		void SetUpvector(glm::vec3 up);

	private:
		glm::mat4 mProjectionMat;

		float mFovY;
		float mAspectRatio;
		float mNear;
		float mFar;

		glm::vec3 mPosition;
		glm::vec3 mTarget;
		glm::vec3 mUp;
	};
}


#endif
