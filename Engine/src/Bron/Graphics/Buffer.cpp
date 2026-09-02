#include "Buffer.h"

#include "Bron/Core/Platform.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Bron
{
	Ref<VertexBuffer> VertexBuffer::Create(usize size)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLVertexBuffer>(size);
		}
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, usize size)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLVertexBuffer>(vertices, size);
		}
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(u32* indices, u32 count)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLIndexBuffer>(indices, count);
		}
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(usize size) {
		switch (Platform::getAPI()) {
			case Platform::API::None:
				CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return createRef<OpenGLIndexBuffer>(size);
		}
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(usize size, u32 binding) {
		switch (Platform::getAPI()) {
			case Platform::API::None:
				CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return createRef<OpenGLUniformBuffer>(size, binding);
		}
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void *data, usize size, u32 binding) {
		switch (Platform::getAPI()) {
			case Platform::API::None:
				CORE_ASSERT(false, "No Rendering API selected!");
			case Platform::API::OpenGL:
				return createRef<OpenGLUniformBuffer>(data, size, binding);
		}
		return nullptr;
	}
} // namespace Bron
