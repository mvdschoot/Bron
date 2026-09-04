#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "API.h"

#include "glm/glm.hpp"

namespace bron {
class BR_API Command {
public:
	static void Init();
	static void ClearColor(const glm::vec4& color);
	static void Clear();

	static void EnableBlend();
	static void EnableDepth();

	static void DrawIndexed(const Ref<VertexArray>& v_array, u32 count);
	static void DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count);
	static void DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count);
	static void OnResize(float x, float y, float width, float height);

private:
	static API* renderer_api_;
};
} // namespace bron
