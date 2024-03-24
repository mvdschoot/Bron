#ifndef LINE_RENDERER_HEADER_
#define LINE_RENDERER_HEADER_

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "../VertexArray.h"
#include "../Buffer.h"
#include "../Shader.h"
#include "../Camera.h"

#include "Command.h"

#include "glm/glm.hpp"

namespace Steve
{

	/*
	 * Draws a plane and fills in the line color if the fragment is close to the line.
	 */
	class GridRenderer
	{
	public:
		static void Init(Camera* camera);
		static void Draw();
	};
}

#endif