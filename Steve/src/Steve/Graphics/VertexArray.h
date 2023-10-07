#ifndef VERTEXARRAY_HEADER
#define VERTEXARRAY_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Graphics/Buffer.h"

#include <vector>
#include <memory>

namespace Steve
{
	class STEVE_API VertexArray
	{
	public:
		VertexArray() = default;

		virtual ~VertexArray()
		{
		};

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void addVertexBuffer(Ref<VertexBuffer> buffer) = 0;
		virtual void setIndexBuffer(Ref<IndexBuffer> buffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() = 0;
		virtual const Ref<IndexBuffer>& getIndexBuffer() = 0;

		static Ref<VertexArray> Create();
	};
} // namespace Steve


#endif
