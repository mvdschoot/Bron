#include "GraphicsContext.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Steve/Core/Platform.h"

namespace Steve
{
	Ref<GraphicsContext> GraphicsContext::Create(GLFWwindow* window)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLContext>(window);
		}
		return nullptr;
	}
}
