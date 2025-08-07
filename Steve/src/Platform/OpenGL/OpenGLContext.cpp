#include "OpenGLContext.h"

#include <iostream>

namespace Steve
{
	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: _graphics_window(window)
	{
		CH_PROFILE_FUNCTION();
		Init();
	}

	void OpenGLContext::Init()
	{
		CH_PROFILE_FUNCTION();
		glfwMakeContextCurrent(_graphics_window);
		CORE_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0, "Cant init glad.");

		glClearColor(1.0, 0.0, 1.0, 0.0);


		auto vendor = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		auto gpu = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		CORE_INFO("Using following GPU:");
		CORE_INFO("	{}", vendor);
		CORE_INFO("	{}", gpu);
	}

	void OpenGLContext::SwapBuffers()
	{
		CH_PROFILE_FUNCTION();
		glfwSwapBuffers(_graphics_window);
	}
}
