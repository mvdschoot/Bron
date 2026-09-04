#include "DesktopWindow.h"
#include "GLFW/glfw3.h"

namespace bron {

DesktopWindow::DesktopWindow(const WindowProps& w_props) : is_glfw_initialised_(false) {
	BR_PROFILE_FUNCTION();
	window_data_.title = w_props.title;
	window_data_.width = w_props.width;
	window_data_.height = w_props.height;

	BR_CORE_INFO("Creating winder {} (w:{} h:{})", window_data_.title, window_data_.width, window_data_.height);

	if (!is_glfw_initialised_) {
		int succes = glfwInit();
		BR_CORE_ASSERT(succes, "GLFW could not be initialised");
		is_glfw_initialised_ = true;
	}

	window_ = glfwCreateWindow(window_data_.width, window_data_.height, window_data_.title.c_str(), nullptr, nullptr);

	BR_CORE_ASSERT(window_, "GLFW could not create a new window");

	glfwSetWindowUserPointer(window_, &window_data_);

	graphics_context_ = GraphicsContext::Create(window_);

	SetVSync(true);

	// Setting up callbacks
	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		WindowResizeEvent evt(width, height);
		data.event_callback(evt);
		data.width = width;
		data.height = height;
	});
	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		WindowCloseEvent event;
		data.event_callback(event);
	});
	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double newx, double newy) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseMovedEvent event((newx), (newy));
		data.event_callback(event);
	});
	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.event_callback(event);
				break;
			}
			default: {
				break;
			}
		}
	});
	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event(key);
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.event_callback(event);
				break;
			}
			default: {
				break;
			}
		}
	});
	glfwSetScrollCallback(window_, [](GLFWwindow* window, double offx, double offy) {
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		MouseScrolledEvent event((offx), (offy));
		data.event_callback(event);
	});
}

void DesktopWindow::OnUpdate() {
	BR_PROFILE_FUNCTION();
	// glViewport(0, 0, width_, height_);
	graphics_context_->SwapBuffers();
	glfwPollEvents();
}

void DesktopWindow::SetVSync(bool enabled) {
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
}

float DesktopWindow::GetMonitorScale() {
	float x_scale, y_scale;
	glfwGetWindowContentScale(this->window_, &x_scale, &y_scale);

	BR_CORE_INFO("Monitor scale x: {} y: {}", x_scale, y_scale);

	if (x_scale == y_scale) {
		return x_scale;
	} else {
		BR_CORE_WARN("The OS's scaling for the primary monitor differs between the x scaling factor and y scaling "
					 "factor: x: {}, y: {}",
					 x_scale, y_scale);
		return x_scale;
	}
}
} // namespace bron
