#include "OpenGLRendererAPI.h"

namespace Steve
{
	OpenGLRendererAPI::OpenGLRendererAPI()
	{
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		CH_PROFILE_FUNCTION();
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear()
	{
		CH_PROFILE_FUNCTION();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::onResize(float x, float y, float width, float height)
	{
		CH_PROFILE_FUNCTION();
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::EnableBlend()
	{
		if (_blend_enabled)
		{
			return;
		}
		_blend_enabled = true;
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::EnableDepth()
	{
		if (!_blend_enabled)
			return;
		_blend_enabled = false;
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void OpenGLRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& v_array, u32 count)
	{
		CH_PROFILE_FUNCTION();
		
		v_array->bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::drawIndexedLines(const std::shared_ptr<VertexArray>& v_array, u32 count)
	{
		CH_PROFILE_FUNCTION();

		u32 c = count ? count : v_array->getIndexBuffer()->getCount();
		v_array->bind();
		glDrawElements(GL_LINES, c, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::drawIndexedStripLines(const std::shared_ptr<VertexArray>& v_array, u32 count)
	{
		CH_PROFILE_FUNCTION();

		u32 c = count ? count : v_array->getIndexBuffer()->getCount();
		v_array->bind();
		glDrawElements(GL_LINE_STRIP, c, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::Init()
	{
		CH_PROFILE_FUNCTION();

	}
}
