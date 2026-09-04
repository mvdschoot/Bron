#include "OpenGLBuffer.h"
#include "Bron/Core/Profiling.h"
#include "GLFW/glfw3.h"

namespace bron
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(usize size)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &render_id_);
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, usize size)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &render_id_);
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::SetBufferData(const void* data, usize size)
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLVertexBuffer::ResizeAndSetBuffer(const void* data, uint64_t size)
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	const BufferLayout& OpenGLVertexBuffer::GetBufferLayout() const
	{
		return layout_;
	}

	void OpenGLVertexBuffer::SetBufferLayout(const BufferLayout& layout)
	{
		layout_ = layout;
	}

	void OpenGLVertexBuffer::Bind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
	}

	void OpenGLVertexBuffer::Unbind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/////////// Indices buffer /////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32 count)
		: count_(0)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &render_id_);
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(u32* indices, u32 count)
		: count_(count)
	{
		BR_PROFILE_FUNCTION();
		glCreateBuffers(1, &render_id_);
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), indices, GL_DYNAMIC_DRAW);
	}

	void OpenGLIndexBuffer::Bind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_id_);
	}

	void OpenGLIndexBuffer::Unbind()
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	u32 OpenGLIndexBuffer::GetCount() const
	{
		return count_;
	}
	void OpenGLIndexBuffer::SetBufferData(u32* data, u32 count)
	{
		Bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(u32), data);
		count_ = count;
	}

	void OpenGLIndexBuffer::ResizeAndSetBuffer(uint32_t* data, uint32_t count)
	{
		BR_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, render_id_);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(u32), data, GL_DYNAMIC_DRAW);
	}

	////////////////////////////////////////////////////////////////////////////////
	//////////////////// Uniform buffer ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	OpenGLUniformBuffer::OpenGLUniformBuffer(usize size, u32 binding) :
		buffer_size(size), current_binding_(binding), bound(false)
	{
		glGenBuffers(1, &renderer_id);
		glBindBuffer(GL_UNIFORM_BUFFER, renderer_id);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, binding, renderer_id, 0, size);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void *data, usize size, u32 binding) :
		buffer_size(size), current_binding_(binding), bound(false)
	{
		glGenBuffers(1, &renderer_id);
		glBindBuffer(GL_UNIFORM_BUFFER, renderer_id);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, binding, renderer_id, 0, size);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &renderer_id);
	}

	void OpenGLUniformBuffer::Bind(u32 binding)
	{
		if (current_binding_ != binding) {
			Unbind();
		}
		current_binding_ = binding;
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, renderer_id);
		bound = true;
	}

	void OpenGLUniformBuffer::Unbind() {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		bound = false;
	}

	bool OpenGLUniformBuffer::IsBound() {
		return bound;
	}

	void OpenGLUniformBuffer::SetData(const u8* data, usize size, usize offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, renderer_id);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::ResizeAndSetData(const u8* data, usize size)
	{
		buffer_size = size;
		glBindBuffer(GL_UNIFORM_BUFFER, renderer_id);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
