#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Window.h"
#include "Bron/Core/Timestep.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Layers/LayerStack.h"
#include "Bron/Layers/Layer.h"
#include "Bron/Layers/ImGuiLayer.h"

#include "Platform/Desktop/DesktopWindow.h"
#include "Bron/Graphics/GraphicsContext.h"

#include "imgui.h"

#include <ctime>
#include <memory>
#include <optional>
#include <random>
#include <string_view>

namespace bron {
/// The arguments main() was started with, handed to CreateApplication so that an
/// application can be told where to work rather than having to guess.
///
/// It points straight at main's argv, which lives for the whole process, so this stays
/// valid without owning anything. values[0] is the executable, as usual - Get() counts
/// from the first real argument instead, because nothing here cares what the binary was
/// called.
struct CommandLineArgs {
	int count = 0;
	char** values = nullptr;

	/// Argument 'index', counting from 0 for the first one after the executable name.
	/// Nothing when there are fewer than that many.
	[[nodiscard]] std::optional<std::string_view> Get(const int index) const {
		const int argv_index = index + 1;
		if (values == nullptr || argv_index < 1 || argv_index >= count)
			return std::nullopt;
		return std::string_view(values[argv_index]);
	}
};

#define BR_BIND_EVENT_FN(fn)                                                                                           \
	[this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define BR_PROFILING_FRAME_COUNT 60
#define BR_PROFILING_START_FRAME (60 * 5)

class BR_API Application {
public:
	Application() : running_(true), minimized_(false), frame_count_(0), profiling_(false) { Init(); }

	// Virtual because main() owns the application through an Application* and deletes it
	// there; without this the derived destructor never runs.
	virtual ~Application() {};
	void Run();
	void SetWindowProps(const WindowProps& props);

	static Ref<Window> GetWindow() { return window_; }

protected:
	/// Adds a layer under every overlay: it draws first and is the last to be offered an
	/// event. Where the thing the application is actually for goes - the editor, a game.
	void PushLayer(Layer* layer);

	/// Adds a layer over everything: it draws last and is the first to be offered an
	/// event. For what covers the application rather than being it - the ImGui layer,
	/// a pause menu, a debug HUD.
	void PushOverlay(Layer* overlay);

private:
	static Ref<Window> window_;
	GraphicsContext* graphics_context_;

	LayerStack layer_stack_;

	// Owned here rather than pushed and forgotten: Run() drives Begin/End around the
	// whole stack, which is not something the stack itself can do.
	Scope<ImGuiLayer> imgui_layer_;

	bool running_, minimized_;
	float last_frame_time_;
	uint64_t frame_count_;
	bool profiling_;

	void Init();
	void OnEvent(Event& event);

	bool OnWindowClose(WindowCloseEvent& event);
	bool OnWindowResize(WindowResizeEvent& event);
};


// To be defined in client app
Application* CreateApplication(CommandLineArgs args);
} // namespace bron
