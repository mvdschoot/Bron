#include "../../Bron/Input/Input.h"

#include "Bron/Core/Application.h"
#include "GLFW/glfw3.h"

namespace bron {
bool Input::IsKeyPressed(KeyCode key) {
	if (blocked_)
		return false;

	GLFWwindow* winder = Application::GetWindow()->GetWindowPointer();
	int state = glfwGetKey(winder, key);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMousePressed(MouseCode code) {
	if (blocked_)
		return false;

	GLFWwindow* winder = Application::GetWindow()->GetWindowPointer();
	int state = glfwGetMouseButton(winder, code);
	return state == GLFW_PRESS;
}

void Input::SetBlocked(bool blocked) { blocked_ = blocked; }
} // namespace bron
