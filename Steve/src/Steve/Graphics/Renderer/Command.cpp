#include "Command.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Steve
{
	void Command::Init()
	{
		_renderer_api->Init();
	}

	API* Command::_renderer_api = new OpenGLRendererAPI();


	void Command::ClearColor(const glm::vec4& color)
	{
		_renderer_api->setClearColor(color);
	}

	void Command::clear()
	{
		_renderer_api->clear();
	}

	void Command::EnableBlend()
	{
		_renderer_api->EnableBlend();
	}

	void Command::EnableDepth()
	{
		_renderer_api->EnableDepth();
	}

	void Command::DrawIndexed(const Ref<VertexArray>& v_array, u32 count)
	{
		_renderer_api->drawIndexed(v_array, count);
	}

	void Command::DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count)
	{
		_renderer_api->drawIndexedLines(v_array, count);
	}

	void Command::DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count)
	{
		_renderer_api->drawIndexedStripLines(v_array, count);
	}

	void Command::onResize(float x, float y, float width, float height)
	{
		_renderer_api->onResize(x, y, width, height);
	}
}
