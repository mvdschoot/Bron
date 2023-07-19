#include "EditorLayer.h"

namespace Steve
{
	void EditorLayer::OnAttach()
	{
		Style = &ImGui::GetStyle();
		SceneHierarchyPanel::OnAttach(&App->Sc);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnEvent(Event& event)
	{
		CORE_INFO(event.GetName())
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
	}

	void EditorLayer::ShowDebug()
	{
		if (ImGui::Begin("Debug")) {
			ImGui::Text("FPS: %f", 1000.0f / App->Ts.getMilliseconds());

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::CollapsingHeader("3D scene renderer", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent();
				ImGui::Text("Shaders: %d", graphics::SceneRenderer::Statistics.Shaders);
				ImGui::Text("Models: %d", graphics::SceneRenderer::Statistics.Models);
				ImGui::Text("Materials: %d", graphics::SceneRenderer::Statistics.Materials);
				ImGui::Text("Meshes: %d", graphics::SceneRenderer::Statistics.Meshes);
				ImGui::Text("Draw calls: %d", graphics::SceneRenderer::Statistics.DrawCalls);
				ImGui::Text("Uniform calls: %d", graphics::SceneRenderer::Statistics.UniformCalls);
			}

			ImGui::End();
		}
	}


	void EditorLayer::OnImGuiRender()
	{
		return;
		using namespace graphics;
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// style.WindowMinSize.x = minWinSizeX;


		// Debug info
		ShowDebug();

		// Viewport
		ImVec2 old_padding = Style->WindowPadding;
		Style->WindowPadding = { 0.0,0.0 };

		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (!compare_float(viewportPanelSize.x, ViewportWindowSize.x) || !compare_float(viewportPanelSize.y, ViewportWindowSize.y))
		{
			App->FrSpec.width = viewportPanelSize.x;
			App->FrSpec.height = viewportPanelSize.y;
			ViewportWindowSize.x = viewportPanelSize.x;
			ViewportWindowSize.y = viewportPanelSize.y;
			App->Framebuffer->invalidate();

		}

		uint64_t textureID = App->Framebuffer->getColorAttachID();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();

		Style->WindowPadding = old_padding;

		// Scene hierarchy
		SceneHierarchyPanel::OnImguiRender();
		
		ImGui::End();
	}
}
