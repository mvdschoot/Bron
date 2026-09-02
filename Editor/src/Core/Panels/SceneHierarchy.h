#pragma once

#include "imgui.h"
#include "ImGuizmo.h"
#include "Bron.h"

namespace Bron::Editor
{
	struct SceneHierarchyPanelData
	{
		Scene* scene = nullptr;
		entt::entity selectedObject = entt::null;
		ImGuizmo::OPERATION selectedObjectOperation = ImGuizmo::OPERATION::TRANSLATE;
	};

	class SceneHierarchyPanel
	{
	public:
		static void OnAttach(Scene* scene);
		static void RenameFunction();
		static void OnImguiRender();
		static void PropertiesPanel();

		static SceneHierarchyPanelData Data;

	private:
		static void TreeNode(entt::entity entity);
	};
}
