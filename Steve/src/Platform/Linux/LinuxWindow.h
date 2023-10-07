#ifndef LINUXWINDOW_HEADER
#define LINUXWINDOW_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Window.h"
#include "Steve/Core/Logger.h"
#include "Steve/Events/Event.h"
#include "Steve/Events/KeyEvent.h"
#include "Steve/Events/WindowEvent.h"
#include "Steve/Events/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Steve/Graphics/GraphicsContext.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <string>

namespace Steve
{
	class LinuxWindow : public Window
	{
	public:
		LinuxWindow(const WindowProps& w_props);

		~LinuxWindow()
		{
		};

		void onUpdate() override;
		void setVSync(bool enabled) override;

		u32 getWindowWidth() override { return _window_data._width; };
		u32 getWindowHeight() override { return _window_data._height; };

		void setEventCallback(const EventCallbackFn& func) override
		{
			_window_data._event_callback = func;
		};

	private:
		bool _is_glfw_initialised;
		Ref<GraphicsContext> _graphics_context;

		WindowData _window_data;
	};
}

#endif
