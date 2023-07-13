#include "OpenGLVertexArray.h"

namespace Steve::graphics
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &_renderer_id);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &_renderer_id);
	}

	void OpenGLVertexArray::bind()
	{
		glBindVertexArray(_renderer_id);
	}

	void OpenGLVertexArray::unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(Ref<VertexBuffer> buffer)
	{
		CH_PROFILE_FUNCTION();
		glBindVertexArray(_renderer_id);
		buffer->bind();

		const BufferLayout& layout = buffer->getBufferLayout();

		for (const BufferElement& el : layout.GetElements())
		{
			glEnableVertexAttribArray(_vertex_buffer_index);
			glVertexAttribPointer(_vertex_buffer_index,
			                      el.getComponentCount(),
			                      ShaderDataTypeToOpenGLType(el.type),
			                      el.normalized ? GL_TRUE : GL_FALSE,
			                      layout.GetStride(),
			                      reinterpret_cast<void*>(el.offset)
			);
			_vertex_buffer_index++;
		}

		_vertex_buffers.push_back(buffer);
	}

	void OpenGLVertexArray::setIndexBuffer(Ref<IndexBuffer> buffer)
	{
		CH_PROFILE_FUNCTION();
		glBindVertexArray(_renderer_id);
		buffer->bind();

		_index_buffer = buffer;
	}

	const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers()
	{
		return _vertex_buffers;
	}

	const Ref<IndexBuffer>& OpenGLVertexArray::getIndexBuffer()
	{
		return _index_buffer;
	}
}
