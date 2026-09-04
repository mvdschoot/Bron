#include "ImGuiLayer.h"

#include "ImGuizmo.h"
#include "Bron/Core/Application.h"

namespace bron
{
	ImGuiLayer::ImGuiLayer(Ref<Window> window) :
		window_(window)
	{
	}

	void ImGuiLayer::OnAttach()
	{
		BR_PROFILE_FUNCTION();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window_->GetWindowPointer(), true);
		ImGui_ImplOpenGL3_Init("#version 130");

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// For 4k
		float scale = Application::GetWindow()->GetMonitorScale();
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

	void ImGuiLayer::Begin()
	{
		BR_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuizmo::BeginFrame();
		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
	}

	void ImGuiLayer::End()
	{
		BR_PROFILE_FUNCTION();
		// Rendering
		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window_->GetWindowPointer(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
