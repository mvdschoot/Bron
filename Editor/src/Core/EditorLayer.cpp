#include "Core/EditorLayer.h"

#include <ImGuizmo.h>

#include "Core/Preferences.h"
#include "Core/Theme.h"
#include "Panels/PreferencesPanel.h"
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
		/// The project to reopen on startup: the most recent one that still loads. Null when
		/// the list is empty or none of it survives, which leaves the editor with no project
		/// open - a normal state, not an error.
		std::unique_ptr<Project> MostRecentProject()
		{
			for (const std::filesystem::path& path : Preferences::Get().recentProjects)
			{
				if (std::unique_ptr<Project> project = Project::Load(path))
					return project;

				// Load has already logged why; a project that has been moved or deleted
				// should not stop the editor from starting.
				CORE_WARN("Skipping {} from the recent projects.", path.string());
			}

			return nullptr;
		}
	}

	template<typename T>
	T* EditorLayer::AddPanel()
	{
		auto panel = createScope<T>(mContext);
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
		mPreferencesPanel = AddPanel<PreferencesPanel>();
	}

	void EditorLayer::OnAttach()
	{
		Command::Init();
		SceneRenderer::Init();
		Command::ClearColor({0.0, 0.0, 0.0, 0.5});
		GridRenderer::Init(&mContext.camera);

		// Reopen where the last session left off. Nothing to reopen is fine: the editor
		// starts with no project, and no asset root, until one is created or opened.
		OpenProject(MostRecentProject());

		// Preferences were read in createApplication(); this is the first point at which
		// there is an ImGui context to apply them to.
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

	void EditorLayer::OpenProject(std::unique_ptr<Project> project)
	{
		// Null when there was nothing to reopen, or when the file could not be read - Load
		// and Create have already logged why. Either way the editor keeps what it has.
		if (!project)
			return;

		mContext.project = std::move(project);
		mContext.project->MakeActive();

		Preferences::AddRecentProject(mContext.project->File());
		Preferences::Save();

		// Always succeeds: a project is guaranteed to have its startup scene on disk.
		mProjectPanel->LoadScene();
	}

	void EditorLayer::OpenProjectDialog()
	{
		NFD::Init();

		NFD::UniquePath outPath;
		nfdfilteritem_t filterItem[1] = {{"Bron projects", "brn"}};

		if (NFD::OpenDialog(outPath, filterItem, 1, nullptr) == NFD_OKAY)
			OpenProject(Project::Load(outPath.get()));

		NFD_Quit();
	}

	void EditorLayer::NewProjectDialog()
	{
		NFD::Init();

		NFD::UniquePath outPath;
		nfdfilteritem_t filterItem[1] = {{"Bron projects", "brn"}};

		if (NFD::SaveDialog(outPath, filterItem, 1, nullptr, "Untitled.brn") == NFD_OKAY)
		{
			const std::filesystem::path file = outPath.get();
			OpenProject(Project::Create(file, file.stem().string()));
		}

		NFD_Quit();
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
			if (ImGui::MenuItem("New project..."))
				NewProjectDialog();

			if (ImGui::MenuItem("Open project..."))
				OpenProjectDialog();

			if (ImGui::BeginMenu("Open recent", !Preferences::Get().recentProjects.empty()))
			{
				// Copied, because opening a project reorders the list being walked.
				const std::vector<std::filesystem::path> recent = Preferences::Get().recentProjects;

				for (const std::filesystem::path& path : recent)
				{
					if (ImGui::MenuItem(path.string().c_str()))
						OpenProject(Project::Load(path));
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			// Everything below needs somewhere to read and write, so it waits for a project.
			ImGui::BeginDisabled(!mContext.HasProject());

			if (ImGui::MenuItem("Save project"))
				mContext.project->Save();

			ImGui::Separator();

			if (ImGui::MenuItem("Save scene"))
				mProjectPanel->SaveScene();
			if (ImGui::MenuItem("Load scene"))
				mProjectPanel->LoadScene();

			ImGui::EndDisabled();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Preferences..."))
				mPreferencesPanel->Open();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
