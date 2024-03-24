#ifndef OPENGLVERTEXARRAY_HEADER
#define OPENGLVERTEXARRAY_HEADER

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Graphics/Buffer.h"
#include "Steve/Graphics/VertexArray.h"

#include <memory>
#include <vector>

namespace Steve
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() override;

		void bind() override;
		void unbind() override;

		void addVertexBuffer(const Ref<VertexBuffer> buffer) override;
		void setIndexBuffer(const Ref<IndexBuffer> buffer) override;

		const std::vector<Ref<VertexBuffer>>& getVertexBuffers() override;
		const Ref<IndexBuffer>& getIndexBuffer() override;

	private:
		u32 _renderer_id;
		std::vector<Ref<VertexBuffer>> _vertex_buffers;
		Ref<IndexBuffer> _index_buffer;

		u32 _vertex_buffer_index = 0;
	};
} // namespace Steve


#endif
