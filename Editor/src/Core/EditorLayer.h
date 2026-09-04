#pragma once

#include <memory>
#include <vector>

#include "Bron.h"

#include "Core/EditorContext.h"
#include "Panels/Panel.h"
#include "Panels/ProjectPanel.h"

namespace Bron::Editor
{
	/// The editor's only layer. It owns the shared state, drives the panels, and provides the
	/// frame they dock into (the dockspace and the main menu bar). Everything specific to one
	/// window lives in a Panel instead.
	class EditorLayer final : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

	private:
		/// Adds a panel and returns it, so the layer can keep a handle on the ones it drives
		/// directly (the menu bar calls into the project panel).
		template<typename T>
		T* AddPanel();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		void PollShortcuts();

		void BeginDockspace();
		void EndDockspace();
		void DrawMenuBar();

		EditorContext mContext;
		std::vector<std::unique_ptr<Panel>> mPanels;

		ProjectPanel* mProjectPanel = nullptr;
	};
}
