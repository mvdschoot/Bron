#include "Buffer.h"

#include "Steve/Core/Platform.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Steve::graphics
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

	Ref<IndexBuffer> IndexBuffer::Create(usize size)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLIndexBuffer>(size);
		}
		return nullptr;
	}
}
