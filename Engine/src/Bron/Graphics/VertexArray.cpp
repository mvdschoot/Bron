#include "VertexArray.h"

#include "Bron/Core/Platform.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace bron
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kOpenGl: return CreateRef<OpenGLVertexArray>();
		case Platform::API::kNone: BR_CORE_ERROR("No rendering engine selected");
		}

		return nullptr;
	}
}
