#include "OpenGLBuffer.h"
#include "Steve/Core/Profiling.h"
#include "GLFW/glfw3.h"

namespace Steve
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(usize size)
	{
		CH_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, usize size)
	{
		CH_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::setBufferData(const void* data, usize size)
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::resizeAndSetBuffer(const void* data, uint64_t size)
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	const BufferLayout& OpenGLVertexBuffer::getBufferLayout() const
	{
		return _layout;
	}

	void OpenGLVertexBuffer::setBufferLayout(const BufferLayout& layout)
	{
		_layout = layout;
	}

	void OpenGLVertexBuffer::bind()
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
	}

	void OpenGLVertexBuffer::unbind()
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/////////// Indices buffer /////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32 count)
		: _count(0)
	{
		CH_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 count)
		: _count(count)
	{
		CH_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), indices, GL_DYNAMIC_DRAW);
	}

	void OpenGLIndexBuffer::bind()
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _render_id);
	}

	void OpenGLIndexBuffer::unbind()
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	u32 OpenGLIndexBuffer::getCount() const
	{
		return _count;
	}
	void OpenGLIndexBuffer::setBufferData(u32* data, u32 count)
	{
		bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(u32), data);
		_count = count;
	}

	void OpenGLIndexBuffer::resizeAndSetBuffer(uint32_t* data, uint32_t count)
	{
		CH_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), data, GL_DYNAMIC_DRAW);
	}
}
