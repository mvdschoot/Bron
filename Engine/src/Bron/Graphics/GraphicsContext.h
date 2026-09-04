#pragma once

#include "Bron/Core/Window.h"
#include "Bron/Core/Core.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace bron
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Ref<GraphicsContext> Create(GLFWwindow* window);
	};
}
