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

namespace bron::editor
{
	namespace
	{
		/// The project to reopen on startup: the most recent one that still loads. Null when
		/// the list is empty or none of it survives, which leaves the editor with no project
		/// open - a normal state, not an error.
		Scope<Project> MostRecentProject()
		{
			for (const std::filesystem::path& path : Preferences::Get().recent_projects)
			{
				if (Scope<Project> project = Project::Load(path))
					return project;

				// Load has already logged why; a project that has been moved or deleted
				// should not stop the editor from starting.
				BR_CORE_WARN("Skipping {} from the recent projects.", path.string());
			}

			return nullptr;
		}
	}

	template<typename T>
	T* EditorLayer::AddPanel()
	{
		auto panel = CreateScope<T>(context_);
		T* raw = panel.get();
		panels_.push_back(std::move(panel));
		return raw;
	}

	EditorLayer::EditorLayer()
	{
		// Declaration order is display order for anything that is not docked yet.
		AddPanel<ViewportPanel>();
		AddPanel<SceneHierarchyPanel>();
		AddPanel<PropertiesPanel>();
		project_panel_ = AddPanel<ProjectPanel>();
		AddPanel<StatisticsPanel>();
		preferences_panel_ = AddPanel<PreferencesPanel>();
	}

	void EditorLayer::OnAttach()
	{
		Command::Init();
		SceneRenderer::Init();
		Command::ClearColor({0.0, 0.0, 0.0, 0.5});
		GridRenderer::Init(&context_.camera);

		// Reopen where the last session left off. Nothing to reopen is fine: the editor
		// starts with no project, and no asset root, until one is created or opened.
		OpenProject(MostRecentProject());

		// Preferences were read in CreateApplication(); this is the first point at which
		// there is an ImGui context to apply them to.
		theme::Apply();

		for (const auto& panel : panels_)
			panel->OnAttach();
	}

	void EditorLayer::OnDetach()
	{
		for (const auto& panel : panels_)
			panel->OnDetach();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(BR_BIND_EVENT_FN(EditorLayer::OnMouseScrolled));

		for (const auto& panel : panels_)
			panel->OnEvent(event);
	}

	bool EditorLayer::OnMouseScrolled(MouseScrolledEvent& e)
	{
		return context_.camera.OnMouseScrolled(e);
	}

	void EditorLayer::OnUpdate(const Timestep ts)
	{
		context_.frame_time = ts;

		PollShortcuts();

		for (const auto& panel : panels_)
			panel->OnUpdate(ts);
	}

	void EditorLayer::PollShortcuts()
	{
		if (Input::IsKeyPressed(key::T))
			context_.gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		if (Input::IsKeyPressed(key::R))
			context_.gizmo_operation = ImGuizmo::OPERATION::ROTATE;
		if (Input::IsKeyPressed(key::H))
			context_.gizmo_operation = ImGuizmo::OPERATION::SCALE;

		// Frame the selection.
		if (Input::IsKeyPressed(key::F) && context_.HasSelection())
			context_.camera.Focus(context_.scene.reg.get<TransformComponent>(context_.selection).Position);
	}

	void EditorLayer::OpenProject(Scope<Project> project)
	{
		// Null when there was nothing to reopen, or when the file could not be read - Load
		// and Create have already logged why. Either way the editor keeps what it has.
		if (!project)
			return;

		context_.project = std::move(project);
		context_.project->MakeActive();

		Preferences::AddRecentProject(context_.project->File());
		Preferences::Save();

		// Always succeeds: a project is guaranteed to have its startup scene on disk.
		project_panel_->LoadScene();
	}

	void EditorLayer::OpenProjectDialog()
	{
		NFD::Init();

		NFD::UniquePath out_path;
		nfdfilteritem_t filter_item[1] = {{"Bron projects", "brn"}};

		if (NFD::OpenDialog(out_path, filter_item, 1, nullptr) == NFD_OKAY)
			OpenProject(Project::Load(out_path.get()));

		NFD_Quit();
	}

	void EditorLayer::NewProjectDialog()
	{
		NFD::Init();

		NFD::UniquePath out_path;
		nfdfilteritem_t filter_item[1] = {{"Bron projects", "brn"}};

		if (NFD::SaveDialog(out_path, filter_item, 1, nullptr, "Untitled.brn") == NFD_OKAY)
		{
			const std::filesystem::path file = out_path.get();
			OpenProject(Project::Create(file, file.stem().string()));
		}

		NFD_Quit();
	}

	void EditorLayer::OnImGuiRender()
	{
		BeginDockspace();
		DrawMenuBar();

		for (const auto& panel : panels_)
			panel->OnImGuiRender();

		EndDockspace();
	}

	void EditorLayer::BeginDockspace()
	{
		// The dockspace host covers the whole viewport and is not itself dockable, because
		// having two docking targets inside each other is confusing.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
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
		ImGui::Begin("Dockspace", &dockspaceOpen, window_flags);

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

			if (ImGui::BeginMenu("Open recent", !Preferences::Get().recent_projects.empty()))
			{
				// Copied, because opening a project reorders the list being walked.
				const std::vector<std::filesystem::path> recent = Preferences::Get().recent_projects;

				for (const std::filesystem::path& path : recent)
				{
					if (ImGui::MenuItem(path.string().c_str()))
						OpenProject(Project::Load(path));
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			// Everything below needs somewhere to read and write, so it waits for a project.
			ImGui::BeginDisabled(!context_.HasProject());

			if (ImGui::MenuItem("Save project"))
				context_.project->Save();

			ImGui::Separator();

			if (ImGui::MenuItem("Save scene"))
				project_panel_->SaveScene();
			if (ImGui::MenuItem("Load scene"))
				project_panel_->LoadScene();

			ImGui::EndDisabled();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Preferences..."))
				preferences_panel_->Open();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
