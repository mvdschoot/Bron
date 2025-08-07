#include "Application.h"

#include "Steve/Graphics/ShaderRegistry.h"

namespace Steve
{
	Ref<Window> Application::_window = nullptr;

	void Application::Run()
	{
		CORE_ASSERT(_window != nullptr, "Window has to be initialised");
		while (_running)
		{
			_frame_count++;
			if (_frame_count == 300)
			{
				CH_PROFILE_BEGIN_SESSION("sesh", "loop.json");
			}

			CH_PROFILE_SCOPE("MAIN_LOOP");
			float time = glfwGetTime();
			Timestep ts(time - _last_frame_time);
			_last_frame_time = time;

			if (!_minimized)
			{
				for (Overlay* overlay : _overlay_stack.getOverlays())
				{
					for (Layer* layer : overlay->getLayers())
					{
						layer->OnUpdate(ts);
					}
				}
			}

			{
				CH_PROFILE_SCOPE("IMGUI_LAYER");
				_imgui_layer->begin();
				for (Overlay* overlay : _overlay_stack.getOverlays())
				{
					for (Layer* layer : overlay->getLayers())
					{
						layer->OnImGuiRender();
					}
				}
				_imgui_layer->end();
			}

			_window->onUpdate();

			if (_frame_count == PROFILING_FRAME_COUNT + PROFILING_START_FRAME)
			{
				CH_PROFILE_END_SESSION();
			}
		}
	}

	void Application::Init()
	{
		srand(time(nullptr));

		WindowProps props;
		props._width = 1280 * 2;
		props._height = 720 * 2;
		_window = Window::Create(props);
		_window->setEventCallback(BIND_EVENT_FN(Application::OnEvent));

		ShaderRegistry::Init();

		auto imgui_overlay = new Overlay;
		auto profiling_overlay = new Overlay;

		_imgui_layer = new ImGuiLayer(_window);
		imgui_overlay->insertLayer(_imgui_layer);

		_overlay_stack.instertOverlay(imgui_overlay);
		_overlay_stack.instertOverlay(profiling_overlay);

		CORE_INFO("Is initialised");
	}

	bool Application::onWindowClose(WindowCloseEvent& event)
	{
		_running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event)
	{
		if (event.getWidth() == 0 || event.getHeight() == 0)
		{
			_minimized = true;
			CORE_INFO("Window is minimized");
		} else
		{
			_minimized = false;
			CORE_INFO("Window size: {}, {}", event.getWidth(), event.getHeight());
		}
		return true;
	}

	void Application::addOverlay(Overlay* overlay)
	{
		_overlay_stack.instertOverlay(overlay);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher disp(event);
		disp.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
		disp.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

		for (auto x = _overlay_stack.getBegin(); x != _overlay_stack.getEnd(); ++x)
		{
			for (auto y = (*x)->getBegin(); y != (*x)->getEnd(); ++y)
			{
				if (event.is_handled)
					return;
				(*y)->OnEvent(event);
			}
		}
	}
}
