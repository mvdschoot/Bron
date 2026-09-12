#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/VertexArray.h"

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace bron {
class API {
public:
	enum class DrawType { kTriangles, kLines };
	enum class StencilFunction { kAlways, kNotEqual };

	virtual void Init() = 0;
	virtual void SetClearColor(const glm::vec4& color) = 0;
	virtual void Clear() = 0;
	virtual void OnResize(float x, float y, float width, float height) = 0;

	virtual void EnableBlend() = 0;
	virtual void EnableDepth() = 0;
	/// Turns the depth test off without touching blending, for overlay passes that must
	/// not be cut into by the geometry they are drawn over.
	virtual void DisableDepth() = 0;

	/// Enables the stencil test. 'reference' is both what the test compares against and
	/// what a passing fragment writes; 'write_mask' of 0 tests without writing.
	virtual void SetStencil(StencilFunction func, u32 reference, u32 write_mask) = 0;
	virtual void DisableStencil() = 0;

	/// Masks colour writes to every attachment at once - for passes that only exist to
	/// fill the depth or stencil buffer.
	virtual void SetColorWrite(bool enabled) = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& v_array, u32 count) = 0;
	virtual void DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count) = 0;
	virtual void DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) = 0;

private:
};
} // namespace bron
