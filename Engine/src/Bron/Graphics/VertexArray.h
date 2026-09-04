#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Graphics/Buffer.h"

#include <vector>
#include <memory>

namespace bron {
class BR_API VertexArray {
public:
	VertexArray() = default;

	virtual ~VertexArray() {};

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void AddVertexBuffer(Ref<VertexBuffer> buffer) = 0;
	virtual void SetIndexBuffer(Ref<IndexBuffer> buffer) = 0;

	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() = 0;
	virtual const Ref<IndexBuffer>& GetIndexBuffer() = 0;

	static Ref<VertexArray> Create();
};
} // namespace bron
