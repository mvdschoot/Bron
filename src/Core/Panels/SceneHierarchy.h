#ifndef __SCENEHIERARCHYPANEL_HEADER__
#define __SCENEHIERARCHYPANEL_HEADER__

#include "imgui.h"
#include "ImGuizmo.h"
#include "Steve/src/Steve.h"

namespace Steve
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

		// Euler angles are only a UI representation of the transform's quaternion; the panel owns the cache
		// so the same quaternion keeps producing the same angles while it is being dragged.
		static void InvalidateEulerCache();

		static SceneHierarchyPanelData Data;

	private:
		static void TreeNode(entt::entity entity);
	};
}


#endif
