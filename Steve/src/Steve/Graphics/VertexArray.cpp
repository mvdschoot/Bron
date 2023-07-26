#include "VertexArray.h"

#include "Steve/Core/Platform.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Steve
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Platform::getAPI())
		{
		case Platform::API::OpenGL: return createRef<OpenGLVertexArray>();
		case Platform::API::None: CORE_ERROR("No rendering engine selected");
		}

		return nullptr;
	}
}
