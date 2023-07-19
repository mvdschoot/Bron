#ifndef __SCENEHIERARCHYPANEL_HEADER__
#define __SCENEHIERARCHYPANEL_HEADER__

#include "Steve/src/Steve.h"

namespace Steve
{
	class SceneHierarchyPanel
	{
	public:
		static void OnAttach(graphics::Scene* scene);
		static void RenameFunction();
		static void OnImguiRender();
	};
}


#endif