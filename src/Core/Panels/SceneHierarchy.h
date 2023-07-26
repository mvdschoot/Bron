#ifndef __SCENEHIERARCHYPANEL_HEADER__
#define __SCENEHIERARCHYPANEL_HEADER__

#include "Steve/src/Steve.h"

namespace Steve
{
	struct SceneHierarchyPanelData
	{
		graphics::Scene* scene;
		Entity* selected;
	};

	class SceneHierarchyPanel
	{
	public:
		static void OnAttach(graphics::Scene* scene);
		static void RenameFunction();
		static void OnImguiRender();
		static void PropertiesPanel();

		static SceneHierarchyPanelData Data;
	};
}


#endif