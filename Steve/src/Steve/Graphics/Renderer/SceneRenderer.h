#ifndef __SCENE_RENDERER__
#define __SCENE_RENDERER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Texture.h"

#include "Steve/Scene/Scene.h"

namespace Steve
{
	struct RenderStatistics
	{
		u32 DrawCalls;
		u32 Shaders;
		u32 Materials;
		u32 Meshes;
		u32 UniformCalls;
	};

	class SceneRenderer
	{
	public:
		static void Init();
		static void Draw(Scene& scene);

		static RenderStatistics Statistics;
	};
}


#endif //__SCENE_RENDERER__