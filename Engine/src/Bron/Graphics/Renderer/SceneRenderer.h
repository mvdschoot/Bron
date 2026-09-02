#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Graphics/Texture.h"

#include "Bron/Scene/Scene.h"

namespace Bron
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
