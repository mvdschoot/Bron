#include "OpenGLContext.h"

#include <iostream>

namespace bron {
OpenGLContext::OpenGLContext(GLFWwindow* window) : graphics_window_(window) {
	BR_PROFILE_FUNCTION();
	Init();
}

void OpenGLContext::Init() {
	BR_PROFILE_FUNCTION();
	glfwMakeContextCurrent(graphics_window_);
	BR_CORE_ASSERT(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) != 0, "Cant init glad.");

	glClearColor(1.0, 0.0, 1.0, 0.0);


	auto vendor = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	auto gpu = std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	BR_CORE_INFO("Using following GPU:");
	BR_CORE_INFO("	{}", vendor);
	BR_CORE_INFO("	{}", gpu);
}

void OpenGLContext::SwapBuffers() {
	BR_PROFILE_FUNCTION();
	glfwSwapBuffers(graphics_window_);
}
} // namespace bron
