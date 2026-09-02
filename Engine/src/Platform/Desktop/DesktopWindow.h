#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Window.h"
#include "Bron/Core/Logger.h"
#include "Bron/Events/Event.h"
#include "Bron/Events/KeyEvent.h"
#include "Bron/Events/WindowEvent.h"
#include "Bron/Events/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Bron/Graphics/GraphicsContext.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <string>

namespace Bron
{
	class DesktopWindow : public Window
	{
	public:
		DesktopWindow(const WindowProps& w_props);

		~DesktopWindow()
		{
		};

		void onUpdate() override;
		void setVSync(bool enabled) override;

		u32 getWindowWidth() override { return _window_data._width; };
		u32 getWindowHeight() override { return _window_data._height; };
		float getMonitorScale() override;

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
