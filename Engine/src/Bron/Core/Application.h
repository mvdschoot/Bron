#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Window.h"
#include "Bron/Core/Timestep.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Layers/OverlayStack.h"
#include "Bron/Layers/Overlay.h"
#include "Bron/Layers/Layer.h"
#include "Bron/Layers/ImGuiLayer.h"

#include "Platform/Desktop/DesktopWindow.h"
#include "Bron/Graphics/GraphicsContext.h"

#include "imgui.h"

#include <ctime>
#include <memory>
#include <random>

namespace bron
{
#define BR_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define PROFILING_FRAME_COUNT 60
#define PROFILING_START_FRAME (60*5)

	class BR_API Application
	{
	public:
		Application() : running_(true), minimized_(false), frame_count_(0), profiling_(false) { Init(); }

		// Virtual because main() owns the application through an Application* and deletes it
		// there; without this the derived destructor never runs.
		virtual ~Application()
		{
		};
		void Run();
		void SetWindowProps(const WindowProps& props);

		static Ref<Window> GetWindow() { return window_; }

	protected:
		void AddOverlay(Overlay* overlay);

	private:
		static Ref<Window> window_;
		GraphicsContext* graphics_context_;

		OverlayStack overlay_stack_;
		ImGuiLayer* imgui_layer_;

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
	Application* CreateApplication();
}
