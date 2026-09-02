#pragma once

#include "Bron.h"

#include "AppLayer.h"

#include "Panels/SceneHierarchy.h"

#include <imgui_spectrum.h>

namespace Bron::Editor
{
	class EditorLayer final : public Layer
	{
	public:
		EditorLayer(AppLayer* app) : App(app) {}

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;
		void ShowDebug();
		
		void OnImGuiRender() override;

		void SetStyle();

	private:
		AppLayer* App;
		ImGuiStyle* Style;

		ImVec2 DebugWindowSize = { 100, 500 };
		ImVec2 ViewportWindowSize = { 500, 500 };
		ImVec2 SceneWindowSize = { 1000, 500 };
	};
}
