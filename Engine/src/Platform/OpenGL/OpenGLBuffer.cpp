#include "OpenGLBuffer.h"
#include "Bron/Core/Profiling.h"
#include "GLFW/glfw3.h"

namespace Bron
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(usize size)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, usize size)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::setBufferData(const void* data, usize size)
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::resizeAndSetBuffer(const void* data, uint64_t size)
	{
		BR_PROFILE_FUNCTION();
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
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
	}

	void OpenGLVertexBuffer::unbind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/////////// Indices buffer /////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32 count)
		: _count(0)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 count)
		: _count(count)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &_render_id);
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), indices, GL_DYNAMIC_DRAW);
	}

	void OpenGLIndexBuffer::bind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _render_id);
	}

	void OpenGLIndexBuffer::unbind()
	{
		BR_PROFILE_FUNCTION();
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
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, _render_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), data, GL_DYNAMIC_DRAW);
	}

	////////////////////////////////////////////////////////////////////////////////
	//////////////////// Uniform buffer ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	OpenGLUniformBuffer::OpenGLUniformBuffer(usize size, u32 binding) :
		bufferSize(size), currentBinding(binding), bound(false)
	{
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, binding, rendererID, 0, size);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void *data, usize size, u32 binding) :
		bufferSize(size), currentBinding(binding), bound(false)
	{
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, binding, rendererID, 0, size);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &rendererID);
	}

	void OpenGLUniformBuffer::bind(u32 binding)
	{
		if (currentBinding != binding) {
			unbind();
		}
		currentBinding = binding;
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, rendererID);
		bound = true;
	}

	void OpenGLUniformBuffer::unbind() {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		bound = false;
	}

	bool OpenGLUniformBuffer::isBound() {
		return bound;
	}

	void OpenGLUniformBuffer::setData(const u8* data, usize size, usize offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::resizeAndSetData(const u8* data, usize size)
	{
		bufferSize = size;
		glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
