#ifndef LINE_RENDERER_HEADER_
#define LINE_RENDERER_HEADER_

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "Shader.h"
#include "Camera.h"

#include "Steve/Graphics/Renderer/Command.h"

#include "glm/glm.hpp"

namespace Steve::graphics
{

	class LineRenderer
	{
	public:
		static void Init();

		static void Start(Camera* camera);
		static void DrawLine(glm::vec3 a, glm::vec3 b, glm::vec4 color, float width);
		static void DrawLineStrip(std::vector<glm::vec3>& vertices, glm::vec4 color, float width);
		static void End();


		static u32 GetLineCount();
		static u32 GetLineIndexCount();
		static u32 GetLineStripCount();
		static u32 GetLineStripIndexCount();
	private:
		static void flush();
	};
}

#endif