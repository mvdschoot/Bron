#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/VertexArray.h"

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace bron
{
	class API
	{
	public:
		enum class DrawType
		{
			kTriangles,
			kLines
		};

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void OnResize(float x, float y, float width, float height) = 0;

		virtual void EnableBlend() = 0;
		virtual void EnableDepth() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& v_array, u32 count) = 0;
		virtual void DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count) = 0;
		virtual void DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) = 0;

	private:
	};
}
