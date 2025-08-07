#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Window.h"
#include "Steve/Core/Timestep.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Layers/OverlayStack.h"
#include "Steve/Layers/Overlay.h"
#include "Steve/Layers/Layer.h"
#include "Steve/Layers/ImGuiLayer.h"

#include "Platform/Linux/LinuxWindow.h"
#include "Steve/Graphics/GraphicsContext.h"

#include "imgui.h"

#include <ctime>
#include <memory>
#include <random>

namespace Steve
{
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define PROFILING_FRAME_COUNT 60
#define PROFILING_START_FRAME (60*5)

	class STEVE_API Application
	{
	public:
		Application() : _running(true), _minimized(false), _frame_count(0), _profiling(false) { Init(); }

		~Application()
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


#endif
