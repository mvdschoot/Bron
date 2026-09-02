#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Events/Event.h"
#include "Bron/Graphics/GraphicsContext.h"

// #include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <functional>
#include <string>


namespace Bron
{
	using EventCallbackFn = std::function<void(Event&)>;

	// Abstraction of window class, is platform dependent
	struct BR_API WindowProps
	{
		std::string _title;
		int _width;
		int _height;

		WindowProps() :
			_title("Default"), _width(1920), _height(1080)
		{
		};

		WindowProps(std::string title, u32 width, u32 height) :
			_title(title), _width(width), _height(height)
		{
		};
	};

	struct WindowData
	{
		std::string _title;
		u32 _width;
		u32 _height;
		EventCallbackFn _event_callback;
	};

	class BR_API Window
	{
	public:
		Window() = default;
		using EventCallbackFn = std::function<void(Event&)>;

		virtual void onUpdate() = 0;

		virtual unsigned int getWindowWidth() = 0;
		virtual unsigned int getWindowHeight() = 0;
		virtual float getMonitorScale() = 0;

		virtual void setEventCallback(const EventCallbackFn& func) = 0;

		virtual void setVSync(bool enabled) = 0;

		GLFWwindow* getWindowPointer()
		{
			return _window;
		};

		static Ref<Window> Create(const WindowProps& w_props = WindowProps());

	protected:
		GLFWwindow* _window;
	private:
	};
}
