#include "VertexArray.h"

#include "Bron/Core/Platform.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace bron
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Platform::GetApi())
		{
		case Platform::API::OpenGL: return CreateRef<OpenGLVertexArray>();
		case Platform::API::None: BR_CORE_ERROR("No rendering engine selected");
		}

		return nullptr;
	}
}
