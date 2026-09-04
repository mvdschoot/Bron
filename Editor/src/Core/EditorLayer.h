#pragma once

#include <memory>
#include <vector>

#include "Bron.h"

#include "Core/EditorContext.h"
#include "Panels/Panel.h"
#include "Panels/PreferencesPanel.h"
#include "Panels/ProjectPanel.h"

namespace bron::editor {
/// The editor's only layer. It owns the shared state, drives the panels, and provides the
/// frame they dock into (the dockspace and the main menu bar). Everything specific to one
/// window lives in a Panel instead.
class EditorLayer final : public Layer {
public:
	EditorLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnEvent(Event& event) override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;

private:
	/// Adds a panel and returns it, so the layer can keep a handle on the ones the menu
	/// bar drives directly.
	template<typename T>
	T* AddPanel();

	/// Makes 'project' the open one: points asset resolution at it, records it in the
	/// recent list and loads its startup scene. Null (a failed load) is ignored.
	void OpenProject(Scope<Project> project);

	/// Writes the open project and its scenes. Nothing to do with no project open.
	void Save();

	/// Ask for a .brn and open it, or create one. No-ops when the dialog is cancelled.
	void OpenProjectDialog();
	void NewProjectDialog();

	bool OnMouseScrolled(MouseScrolledEvent& e);
	void PollShortcuts();

	void BeginDockspace();
	void EndDockspace();
	void DrawMenuBar();

	EditorContext context_;
	std::vector<Scope<Panel>> panels_;

	ProjectPanel* project_panel_ = nullptr;
	PreferencesPanel* preferences_panel_ = nullptr;
};
} // namespace bron::editor
