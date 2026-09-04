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
			for (Overlay* overlay: overlay_stack_.GetOverlays()) {
				for (Layer* layer: overlay->GetLayers()) {
					layer->OnUpdate(ts);
				}
			}
		}

		{
			BR_PROFILE_SCOPE("IMGUI_LAYER");
			imgui_layer_->Begin();
			for (Overlay* overlay: overlay_stack_.GetOverlays()) {
				for (Layer* layer: overlay->GetLayers()) {
					layer->OnImGuiRender();
				}
			}
			imgui_layer_->End();
		}

		window_->OnUpdate();

		if (frame_count_ == BR_PROFILING_FRAME_COUNT + BR_PROFILING_START_FRAME) {
			BR_PROFILE_END_SESSION();
		}
	}
}

void Application::Init() {
	srand(time(nullptr));

	WindowProps props;
	props.width = 1280 * 2;
	props.height = 720 * 2;
	window_ = Window::Create(props);
	window_->SetEventCallback(BR_BIND_EVENT_FN(Application::OnEvent));

	ShaderRegistry::Init();

	auto imgui_overlay = new Overlay;
	auto profiling_overlay = new Overlay;

	imgui_layer_ = new ImGuiLayer(window_);
	imgui_overlay->InsertLayer(imgui_layer_);

	overlay_stack_.InsertOverlay(imgui_overlay);
	overlay_stack_.InsertOverlay(profiling_overlay);

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

void Application::AddOverlay(Overlay* overlay) { overlay_stack_.InsertOverlay(overlay); }

void Application::OnEvent(Event& event) {
	EventDispatcher disp(event);
	disp.Dispatch<WindowCloseEvent>(BR_BIND_EVENT_FN(Application::OnWindowClose));
	disp.Dispatch<WindowResizeEvent>(BR_BIND_EVENT_FN(Application::OnWindowResize));

	for (auto x = overlay_stack_.GetBegin(); x != overlay_stack_.GetEnd(); ++x) {
		for (auto y = (*x)->GetBegin(); y != (*x)->GetEnd(); ++y) {
			if (event.is_handled)
				return;
			(*y)->OnEvent(event);
		}
	}
}
} // namespace bron
