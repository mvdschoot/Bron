#include "GraphicsContext.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Bron/Core/Platform.h"

namespace bron
{
	Ref<GraphicsContext> GraphicsContext::Create(GLFWwindow* window)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::kNone: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::kOpenGl: return CreateRef<OpenGLContext>(window);
		}
		return nullptr;
	}
}
