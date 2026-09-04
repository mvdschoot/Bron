#include "OrthographicCamera.h"

namespace bron
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio)
		: AspectRatio(aspectRatio), view_mat_(glm::mat4(1.0f)), projection_mat_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), zoom_level_(1.0f)
	{
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float aspectRatio,
	                                       glm::vec3 position, float rotation)
		: AspectRatio(aspectRatio), projection_mat_(glm::mat4(1.0f)), view_mat_(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), position_(position), target_({0.0f, 0.0f, 0.0f}), rotation_(rotation),
		  zoom_level_(1.0f)
	{
		Recalculate();
	}

	void OrthographicCamera::SetProjection(float left, float right, float top, float bottom)
	{
		projection_mat_ = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void OrthographicCamera::SetPosition(glm::vec3 pos)
	{
		position_ = pos;
		Recalculate();
	}

	void OrthographicCamera::SetRotation(const float angle)
	{
		rotation_ = angle;
		Recalculate();
	}

	void OrthographicCamera::SetZoom(const float zoom)
	{
		zoom_level_ = zoom;
		SetProjection(-AspectRatio * zoom_level_, AspectRatio * zoom_level_, zoom_level_, -zoom_level_);
	}

	void OrthographicCamera::Recalculate()
	{
		glm::mat4 transform = translate(glm::mat4(1.0f), position_) *
			rotate(glm::mat4(1.0f), rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
		view_mat_ = inverse(transform);
	}
}
