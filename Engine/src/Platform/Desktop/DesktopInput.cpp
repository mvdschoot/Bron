#include "Bron/Core/Input.h"

#include "Bron/Core/Application.h"
#include "GLFW/glfw3.h"

namespace Bron
{
	bool Input::isKeyPressed(KeyCode key)
	{
		GLFWwindow* winder = Application::getWindow()->getWindowPointer();
		int state = glfwGetKey(winder, key);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMousePressed(MouseCode code)
	{
		GLFWwindow* winder = Application::getWindow()->getWindowPointer();
		int state = glfwGetMouseButton(winder, code);
		return state == GLFW_PRESS;
	}
}
