#include "ImGuiLayer.h"

#include "ImGuizmo.h"
#include "Steve/Core/Application.h"

namespace Steve
{
	ImGuiLayer::ImGuiLayer(Ref<Window> window) :
		_window(window)
	{
	}

	void ImGuiLayer::OnAttach()
	{
		CH_PROFILE_FUNCTION();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(_window->getWindowPointer(), true);
		ImGui_ImplOpenGL3_Init("#version 130");

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// For 4k
		float scale = Application::getWindow()->getMonitorScale();
		ImGui::GetStyle().FontScaleMain *= scale;
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

	void ImGuiLayer::OnUpdate(Timestep ts)
	{
	}

	void ImGuiLayer::begin()
	{
		CH_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuizmo::BeginFrame();
		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
	}

	void ImGuiLayer::end()
	{
		CH_PROFILE_FUNCTION();
		// Rendering
		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(_window->getWindowPointer(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
