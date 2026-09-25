#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Window.h"
#include "Bron/Core/Logger.h"
#include "Bron/Input/Event.h"
#include "Bron/Input/KeyEvent.h"
#include "Bron/Input/WindowEvent.h"
#include "Bron/Input/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Bron/Graphics/GraphicsContext.h"

#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <string>

namespace bron {
class DesktopWindow : public Window {
public:
	DesktopWindow(const WindowProps& w_props);

	~DesktopWindow() {};

	void OnUpdate() override;
	void SetVSync(bool enabled) override;

	u32 GetWindowWidth() override { return window_data_.width; };
	u32 GetWindowHeight() override { return window_data_.height; };
	float GetMonitorScale() override;

	void SetEventCallback(const EventCallbackFn& func) override { window_data_.event_callback = func; };

private:
	bool is_glfw_initialised_;
	Ref<GraphicsContext> graphics_context_;

	WindowData window_data_;
};
} // namespace bron
