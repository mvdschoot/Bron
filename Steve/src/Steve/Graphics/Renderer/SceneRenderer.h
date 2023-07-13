#ifndef __SCENE_RENDERER__
#define __SCENE_RENDERER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Texture.h"

#include "Steve/Graphics/Scene/Scene.h"

namespace Steve::graphics
{
	class SceneRenderer
	{
	public:
		static void Init();
		static void Draw(Scene& scene);
	};
}


#endif //__SCENE_RENDERER__