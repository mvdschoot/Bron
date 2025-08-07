#ifndef __SCENEHIERARCHYPANEL_HEADER__
#define __SCENEHIERARCHYPANEL_HEADER__

#include "ImGuizmo.h"
#include "Steve/src/Steve.h"

namespace Steve
{
	struct SceneHierarchyPanelData
	{
		Scene* scene;
		Node* selectedObject;
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
		static void TreeNode(Node* node);
		static bool SkipNode(Node* entity);
	};
}


#endif