#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/VertexArray.h"

#include <memory>
#include <vector>

namespace bron {
class OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	~OpenGLVertexArray() override;

	void Bind() override;
	void Unbind() override;

	void AddVertexBuffer(const Ref<VertexBuffer> buffer) override;
	void SetIndexBuffer(const Ref<IndexBuffer> buffer) override;

	const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() override;
	const Ref<IndexBuffer>& GetIndexBuffer() override;

private:
	u32 renderer_id_;
	std::vector<Ref<VertexBuffer>> vertex_buffers_;
	Ref<IndexBuffer> index_buffer_;

	u32 vertex_buffer_index_ = 0;
};
} // namespace bron
