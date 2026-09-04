#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Events/Event.h"
#include "Bron/Graphics/GraphicsContext.h"

// #include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <functional>
#include <string>


namespace bron {
using EventCallbackFn = std::function<void(Event&)>;

// Abstraction of window class, is platform dependent
struct BR_API WindowProps {
	std::string title;
	int width;
	int height;

	WindowProps() : title("Default"), width(1920), height(1080) {};

	WindowProps(std::string title, u32 width, u32 height) : title(title), width(width), height(height) {};
};

struct WindowData {
	std::string title;
	u32 width;
	u32 height;
	EventCallbackFn event_callback;
};

class BR_API Window {
public:
	Window() = default;
	using EventCallbackFn = std::function<void(Event&)>;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWindowWidth() = 0;
	virtual unsigned int GetWindowHeight() = 0;
	virtual float GetMonitorScale() = 0;

	virtual void SetEventCallback(const EventCallbackFn& func) = 0;

	virtual void SetVSync(bool enabled) = 0;

	GLFWwindow* GetWindowPointer() { return window_; };

	static Ref<Window> Create(const WindowProps& w_props = WindowProps());

protected:
	GLFWwindow* window_;

private:
};
} // namespace bron
