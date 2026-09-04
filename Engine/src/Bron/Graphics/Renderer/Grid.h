#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "../VertexArray.h"
#include "../Buffer.h"
#include "../Shader.h"
#include "../Camera.h"

#include "Command.h"

#include "glm/glm.hpp"

namespace bron {

/*
 * Draws a plane and fills in the line color if the fragment is close to the line.
 */
class GridRenderer {
public:
	static void Init(Camera* camera);
	static void Draw();
};
} // namespace bron
