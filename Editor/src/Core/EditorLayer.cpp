#include "Core/EditorLayer.h"

#include <ImGuizmo.h>

#include "Core/Theme.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/StatisticsPanel.h"
#include "Panels/ViewportPanel.h"

#include "Bron/Util/Paths.h"
#include "nfd.hpp"

namespace Bron::Editor
{
	namespace
	{
		/// Contents of a freshly opened editor, until opening a project replaces it.
		void PopulateDefaultScene(Scene& scene)
		{
			scene.CreatePhongModel("The model", Paths::ProjectAssetString("mymodel/untitled.glb").c_str());

			scene.CreatePointLight({4.0, 2.0, 4.0}, {1.0, 1.0, 1.0});
			scene.CreatePointLight({-2.0, 2.0, -2.0}, {1.0, 1.0, 1.0});
		}
	}

	template<typename T>
	T* EditorLayer::AddPanel()
	{
		auto panel = std::make_unique<T>(mContext);
		T* raw = panel.get();
		mPanels.push_back(std::move(panel));
		return raw;
	}

	EditorLayer::EditorLayer()
	{
		// Declaration order is display order for anything that is not docked yet.
		AddPanel<ViewportPanel>();
		AddPanel<SceneHierarchyPanel>();
		AddPanel<PropertiesPanel>();
		mProjectPanel = AddPanel<ProjectPanel>();
		AddPanel<StatisticsPanel>();
	}

	void EditorLayer::OnAttach()
	{
		Command::Init();
		SceneRenderer::Init();
		Command::ClearColor({0.0, 0.0, 0.0, 0.5});
		GridRenderer::Init(&mContext.camera);

		PopulateDefaultScene(mContext.scene);

		Theme::Apply();

		for (const auto& panel : mPanels)
			panel->OnAttach();
	}

	void EditorLayer::OnDetach()
	{
		for (const auto& panel : mPanels)
			panel->OnDetach();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(EditorLayer::OnMouseScrolled));

		for (const auto& panel : mPanels)
			panel->OnEvent(event);
	}

	bool EditorLayer::OnMouseScrolled(MouseScrolledEvent& e)
	{
		return mContext.camera.OnMouseScrolled(e);
	}

	void EditorLayer::OnUpdate(const Timestep ts)
	{
		mContext.frameTime = ts;

		PollShortcuts();

		for (const auto& panel : mPanels)
			panel->OnUpdate(ts);
	}

	void EditorLayer::PollShortcuts()
	{
		if (Input::isKeyPressed(Key::T))
			mContext.gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		if (Input::isKeyPressed(Key::R))
			mContext.gizmoOperation = ImGuizmo::OPERATION::ROTATE;
		if (Input::isKeyPressed(Key::H))
			mContext.gizmoOperation = ImGuizmo::OPERATION::SCALE;

		// Frame the selection.
		if (Input::isKeyPressed(Key::F) && mContext.HasSelection())
			mContext.camera.Focus(mContext.scene.reg.get<TransformComponent>(mContext.selection).Position);
	}

	void EditorLayer::OnImGuiRender()
	{
		BeginDockspace();
		DrawMenuBar();

		for (const auto& panel : mPanels)
			panel->OnImGuiRender();

		EndDockspace();
	}

	void EditorLayer::BeginDockspace()
	{
		// The dockspace host covers the whole viewport and is not itself dockable, because
		// having two docking targets inside each other is confusing.
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// Proceed even if Begin() returns false (the window is collapsed): the DockSpace() call
		// has to keep happening, or every window docked into it loses its parent.
		static bool dockspaceOpen = true;
		ImGui::Begin("Dockspace", &dockspaceOpen, windowFlags);

		ImGui::PopStyleVar(3);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
			ImGui::DockSpace(ImGui::GetID("EditorDockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	void EditorLayer::EndDockspace()
	{
		ImGui::End();
	}

	void EditorLayer::DrawMenuBar()
	{
		if (!ImGui::BeginMainMenuBar())
			return;

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open project..."))
			{
				NFD::Init();

				NFD::UniquePath outPath;
				nfdfilteritem_t filterItem[1] = {{"Bron Editor files", "brn"}};

				if (NFD::OpenDialog(outPath, filterItem, 1, nullptr) == NFD_OKAY)
				{
					// TODO: hand the path to Project once it can load one.
				}

				NFD_Quit();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save scene"))
				mProjectPanel->SaveScene();
			if (ImGui::MenuItem("Load scene"))
				mProjectPanel->LoadScene();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
