#ifndef RENDERERAPI_HEADER
#define RENDERERAPI_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Steve/Graphics/VertexArray.h"

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace Steve
{
	class API
	{
	public:
		enum class DrawType
		{
			TRIANGLES,
			LINES
		};

		virtual void Init() = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void onResize(float x, float y, float width, float height) = 0;

		virtual void EnableBlend() = 0;
		virtual void EnableDepth() = 0;

		virtual void drawIndexed(const Ref<VertexArray>& v_array, u32 count) = 0;
		virtual void drawIndexedLines(const Ref<VertexArray>& v_array, u32 count) = 0;
		virtual void drawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) = 0;

	private:
	};
}


#endif
