#include "LinuxWindow.h"
#include "GLFW/glfw3.h"

namespace Steve
{
	LinuxWindow::LinuxWindow(const WindowProps& w_props) : _is_glfw_initialised(false)
	{
		CH_PROFILE_FUNCTION();
		_window_data._title = w_props._title;
		_window_data._width = w_props._width;
		_window_data._height = w_props._height;

		CORE_INFO("Creating winder {} (w:{} h:{})", _window_data._title, _window_data._width, _window_data._height);

		if (!_is_glfw_initialised)
		{
			int succes = glfwInit();
			CORE_ASSERT(succes, "GLFW could not be initialised");
			_is_glfw_initialised = true;
		}

		_window = glfwCreateWindow(_window_data._width,
		                           _window_data._height,
		                           _window_data._title.c_str(),
		                           nullptr,
		                           nullptr
		);

		CORE_ASSERT(_window, "GLFW could not create a new window");

		glfwSetWindowUserPointer(_window, &_window_data);

		_graphics_context = GraphicsContext::Create(_window);

		setVSync(true);

		// Setting up callbacks 
		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			WindowResizeEvent evt(width, height);
			data._event_callback(evt);
			data._width = width;
			data._height = height;
		});
		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			WindowCloseEvent event;
			data._event_callback(event);
		});
		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double newx, double newy)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent event((newx), (newy));
			data._event_callback(event);
		});
		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action)
			{
			case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data._event_callback(event);
					break;
				}
			case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data._event_callback(event);
					break;
				}
			default:
				{
					break;
				}
			}
		});
		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action)
			{
			case GLFW_PRESS:
				{
					KeyPressedEvent event(key);
					data._event_callback(event);
					break;
				}
			case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data._event_callback(event);
					break;
				}
			default:
				{
					break;
				}
			}
		});
		glfwSetScrollCallback(_window, [](GLFWwindow* window, double offx, double offy)
		{
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseScrolledEvent event((offx), (offy));
			data._event_callback(event);
		});
	}

	void LinuxWindow::onUpdate()
	{
		CH_PROFILE_FUNCTION();
		// glViewport(0, 0, _width, _height);
		_graphics_context->SwapBuffers();
		glfwPollEvents();
	}

	void LinuxWindow::setVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
}
