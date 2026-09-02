#include "EditorLayer.h"

#include <ImGuizmo.h>
#include "IconManagement.h"
#include "Panels/ComponentRegistry.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Bron::Editor
{
	void EditorLayer::OnAttach()
	{
		Style = &ImGui::GetStyle();
		SceneHierarchyPanel::OnAttach(&App->Sc);
		Icons::Load();
		SetStyle();
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnEvent(Event& event)
	{
		// CORE_INFO(event.GetName());
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
				ImGui::Text("Shaders: %d", SceneRenderer::Statistics.Shaders);
				ImGui::Text("Materials: %d", SceneRenderer::Statistics.Materials);
				ImGui::Text("Meshes: %d", SceneRenderer::Statistics.Meshes);
				ImGui::Text("Draw calls: %d", SceneRenderer::Statistics.DrawCalls);
				ImGui::Text("Uniform calls: %d", SceneRenderer::Statistics.UniformCalls);
			}

			ImGui::End();
		}
	}


	void EditorLayer::OnImGuiRender()
	{
		
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
			App->mFramebuffer->invalidate();

			// The projection has to follow the panel, otherwise the scene is stretched to fit it.
			if (viewportPanelSize.y > 0.0f)
				App->Camera->SetAspectRatio(viewportPanelSize.x / viewportPanelSize.y);

		}

		uint64_t textureID = App->mFramebuffer->getColorAttachID();
		ImGui::Image(textureID, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Guizmo
		const entt::entity selected = SceneHierarchyPanel::Data.selectedObject;
		if (selected != entt::null)
		{
			Scene& scene = App->Sc;

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportOffset = ImGui::GetWindowPos();
			ImGuizmo::SetRect(viewportMinRegion.x + viewportOffset.x,
							  viewportMinRegion.y + viewportOffset.y,
							  ViewportWindowSize.x, ViewportWindowSize.y);

			glm::mat4 proj = scene.camera->GetProjectionMatrix();
			glm::mat4 view = scene.camera->GetViewMatrix();

			TransformComponent& comp = scene.reg.get<TransformComponent>(selected);

			// The gizmo manipulates a world transform; the component stores a local one.
			glm::mat4 transform = scene.WorldTransform(selected);

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
								 SceneHierarchyPanel::Data.selectedObjectOperation,
								 ImGuizmo::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing()) // only update if the user is manipulating
			{
				// Back out the parent transform, so the entity keeps its place in the hierarchy.
				const entt::entity parent = scene.reg.get<HierarchyComponent>(selected).parent;
				glm::mat4 parent_transform = parent != entt::null ? scene.WorldTransform(parent) : glm::mat4(1.0f);
				glm::mat4 local = glm::inverse(parent_transform) * transform;

				// Extract TRS in a stable way
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::quat rotationQuat;
				glm::decompose(local, comp.Scaling, rotationQuat, comp.Position, skew, perspective);
				glm::quat newQuat = glm::normalize(rotationQuat);
				if (glm::dot(rotationQuat, comp.RotationQuat) < 0.0f)
					newQuat = -newQuat;

				comp.RotationQuat = newQuat;

				// The properties panel caches euler angles; the gizmo just changed the quaternion under it.
				ComponentRegistry::InvalidateEulerCache();
			}
		}


		ImGui::End();

		Style->WindowPadding = old_padding;

		// Scene hierarchy
		SceneHierarchyPanel::OnImguiRender();
		
		ImGui::End();
	}

	void EditorLayer::SetStyle()
	{
		using namespace ImGui;
		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	}
}
