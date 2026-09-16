#include "Application.h"

#include "Bron/Graphics/ShaderRegistry.h"

namespace bron {
Ref<Window> Application::window_ = nullptr;

void Application::Run() {
	BR_CORE_ASSERT(window_ != nullptr, "Window has to be initialised");
	while (running_) {
		frame_count_++;
		if (frame_count_ == 300) {
			BR_PROFILE_BEGIN_SESSION("sesh", "loop.json");
		}

		BR_PROFILE_SCOPE("MAIN_LOOP");
		float time = glfwGetTime();
		Timestep ts(time - last_frame_time_);
		last_frame_time_ = time;

		if (!minimized_) {
			// Front to back, so an overlay draws over what it covers.
			for (Layer* layer: layer_stack_)
				layer->OnUpdate(ts);
		}

		{
			BR_PROFILE_SCOPE("IMGUI_LAYER");
			imgui_layer_->Begin();
			for (Layer* layer: layer_stack_)
				layer->OnImGuiRender();
			imgui_layer_->End();
		}

		window_->OnUpdate();

		if (frame_count_ == BR_PROFILING_FRAME_COUNT + BR_PROFILING_START_FRAME) {
			BR_PROFILE_END_SESSION();
		}
	}

	// Here rather than in a destructor: the window and its graphics context are still up
	// at this point, and a layer releasing a texture needs them to be.
	layer_stack_.DetachAll();
}

void Application::Init() {
	srand(time(nullptr));

	WindowProps props;
	props.width = 1280 * 2;
	props.height = 720 * 2;
	window_ = Window::Create(props);
	window_->SetEventCallback(BR_BIND_EVENT_FN(Application::OnEvent));

	ShaderRegistry::Init();

	imgui_layer_ = CreateScope<ImGuiLayer>(window_);

	// An overlay, so it covers whatever the application pushes later and is offered
	// every event before that layer is.
	layer_stack_.PushOverlay(imgui_layer_.get());

	BR_CORE_INFO("Is initialised");
}

bool Application::OnWindowClose(WindowCloseEvent& event) {
	running_ = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& event) {
	if (event.GetWidth() == 0 || event.GetHeight() == 0) {
		minimized_ = true;
		BR_CORE_INFO("Window is minimized");
	} else {
		minimized_ = false;
		BR_CORE_INFO("Window size: {}, {}", event.GetWidth(), event.GetHeight());
	}
	return true;
}

void Application::PushLayer(Layer* layer) { layer_stack_.PushLayer(layer); }

void Application::PushOverlay(Layer* overlay) { layer_stack_.PushOverlay(overlay); }

void Application::OnEvent(Event& event) {
	EventDispatcher disp(event);
	disp.Dispatch<WindowCloseEvent>(BR_BIND_EVENT_FN(Application::OnWindowClose));
	disp.Dispatch<WindowResizeEvent>(BR_BIND_EVENT_FN(Application::OnWindowResize));

	// Back to front, the reverse of the draw order: what is on top is offered the event
	// first, and once it marks the event handled nothing underneath it sees it.
	for (auto it = layer_stack_.rbegin(); it != layer_stack_.rend(); ++it) {
		if (event.is_handled)
			return;

		(*it)->OnEvent(event);
	}
}
} // namespace bron
