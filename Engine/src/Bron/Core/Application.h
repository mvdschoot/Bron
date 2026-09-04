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

namespace Bron
{
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define PROFILING_FRAME_COUNT 60
#define PROFILING_START_FRAME (60*5)

	class BR_API Application
	{
	public:
		Application() : _running(true), _minimized(false), _frame_count(0), _profiling(false) { Init(); }

		// Virtual because main() owns the application through an Application* and deletes it
		// there; without this the derived destructor never runs.
		virtual ~Application()
		{
		};
		void Run();
		void setWindowProps(const WindowProps& props);

		static Ref<Window> getWindow() { return _window; }

	protected:
		void addOverlay(Overlay* overlay);

	private:
		static Ref<Window> _window;
		GraphicsContext* _graphics_context;

		OverlayStack _overlay_stack;
		ImGuiLayer* _imgui_layer;

		bool _running, _minimized;
		float _last_frame_time;
		uint64_t _frame_count;
		bool _profiling;

		void Init();
		void OnEvent(Event& event);

		bool onWindowClose(WindowCloseEvent& event);
		bool onWindowResize(WindowResizeEvent& event);
	};


	// To be defined in client app
	Application* createApplication();
}
