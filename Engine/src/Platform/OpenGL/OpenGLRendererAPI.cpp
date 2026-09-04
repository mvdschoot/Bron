#include "OpenGLRendererAPI.h"

namespace bron
{
	OpenGLRendererAPI::OpenGLRendererAPI()
	{
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		BR_PROFILE_FUNCTION();
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		BR_PROFILE_FUNCTION();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::OnResize(float x, float y, float width, float height)
	{
		BR_PROFILE_FUNCTION();
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::EnableBlend()
	{
		if (blend_enabled_)
		{
			return;
		}
		blend_enabled_ = true;
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::EnableDepth()
	{
		if (!blend_enabled_)
			return;
		blend_enabled_ = false;
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& v_array, u32 count)
	{
		BR_PROFILE_FUNCTION();
		
		v_array->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count)
	{
		BR_PROFILE_FUNCTION();

		u32 c = count ? count : v_array->GetIndexBuffer()->GetCount();
		v_array->Bind();
		glDrawElements(GL_LINES, c, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count)
	{
		BR_PROFILE_FUNCTION();

		u32 c = count ? count : v_array->GetIndexBuffer()->GetCount();
		v_array->Bind();
		glDrawElements(GL_LINE_STRIP, c, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::Init()
	{
		BR_PROFILE_FUNCTION();

	}
}
