#include "OrthographicCamera.h"

namespace Bron
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio)
		: AspectRatio(aspectRatio), mViewMat(glm::mat4(1.0f)), mProjectionMat(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), mZoomLevel(1.0f)
	{
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio,
	                                       glm::vec3 position, float rotation)
		: AspectRatio(aspectRatio), mProjectionMat(glm::mat4(1.0f)), mViewMat(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), mPosition(position), mTarget({0.0f, 0.0f, 0.0f}), mRotation(rotation),
		  mZoomLevel(1.0f)
	{
		recalculate();
	}

	void OrthographicCamera::SetProjection(float left, float right, float top, float bottom)
	{
		mProjectionMat = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void OrthographicCamera::SetPosition(glm::vec3 pos)
	{
		mPosition = pos;
		recalculate();
	}

	void OrthographicCamera::SetRotation(const float angle)
	{
		mRotation = angle;
		recalculate();
	}

	void OrthographicCamera::SetZoom(const float zoom)
	{
		mZoomLevel = zoom;
		SetProjection(-AspectRatio * mZoomLevel, AspectRatio * mZoomLevel, mZoomLevel, -mZoomLevel);
	}

	void OrthographicCamera::recalculate()
	{
		glm::mat4 transform = translate(glm::mat4(1.0f), mPosition) *
			rotate(glm::mat4(1.0f), mRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		mViewMat = inverse(transform);
	}
}
