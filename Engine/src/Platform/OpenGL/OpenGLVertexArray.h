#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/VertexArray.h"

#include <memory>
#include <vector>

namespace Bron
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
} // namespace Bron
