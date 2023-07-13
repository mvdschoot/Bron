#ifndef OPENGLGRAPHICSCONTEXT_HEADER
#define OPENGLGRAPHICSCONTEXT_HEADER

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Steve/Events/Event.h"
#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/GraphicsContext.h"

#include <string>

namespace Steve::graphics
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
		GLFWwindow* _graphics_window;
	};
}

#endif
