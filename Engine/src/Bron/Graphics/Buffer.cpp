#include "Buffer.h"

#include "Bron/Core/Platform.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace bron
{
	Ref<VertexBuffer> VertexBuffer::Create(usize size)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::None: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, usize size)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::None: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(u32* indices, u32 count)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::None: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
		}
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(usize size) {
		switch (Platform::GetApi()) {
			case Platform::API::None:
				BR_CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return CreateRef<OpenGLIndexBuffer>(size);
		}
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(usize size, u32 binding) {
		switch (Platform::GetApi()) {
			case Platform::API::None:
				BR_CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
		}
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void *data, usize size, u32 binding) {
		switch (Platform::GetApi()) {
			case Platform::API::None:
				BR_CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(data, size, binding);
		}
		return nullptr;
	}
} // namespace bron
