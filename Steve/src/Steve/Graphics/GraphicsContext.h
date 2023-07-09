#ifndef GRAPHICSCONTEXT_HEADER
#define GRAPHICSCONTEXT_HEADER

#include "Steve/Core/Window.h"
#include "Steve/Core/Core.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Steve::graphics
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Ref<GraphicsContext> Create(GLFWwindow* window);
	};
}

#endif
