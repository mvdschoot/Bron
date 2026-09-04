#pragma once

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Bron/Events/Event.h"
#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Graphics/GraphicsContext.h"

#include <string>

namespace bron
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		~OpenGLContext()
		{
		}

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* graphics_window_;
	};
}
