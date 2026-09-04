#include "OpenGLVertexArray.h"

namespace bron
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &renderer_id_);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &renderer_id_);
	}

	void OpenGLVertexArray::Bind()
	{
		glBindVertexArray(renderer_id_);
	}

	void OpenGLVertexArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> buffer)
	{
		BR_PROFILE_FUNCTION();
		glBindVertexArray(renderer_id_);
		buffer->Bind();

		const BufferLayout& layout = buffer->GetBufferLayout();

		for (const BufferElement& el : layout.GetElements())
		{
			glEnableVertexAttribArray(vertex_buffer_index_);
			glVertexAttribPointer(vertex_buffer_index_,
			                      el.GetComponentCount(),
			                      ShaderDataTypeToOpenGLType(el.type),
			                      el.normalized ? GL_TRUE : GL_FALSE,
			                      layout.GetStride(),
			                      reinterpret_cast<void*>(el.offset)
			);
			vertex_buffer_index_++;
		}

		vertex_buffers_.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> buffer)
	{
		BR_PROFILE_FUNCTION();
		glBindVertexArray(renderer_id_);
		buffer->Bind();

		index_buffer_ = buffer;
	}

	const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers()
	{
		return vertex_buffers_;
	}

	const Ref<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer()
	{
		return index_buffer_;
	}
}
