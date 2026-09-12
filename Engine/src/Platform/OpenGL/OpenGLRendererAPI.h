#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "Bron/Graphics/Renderer/API.h"
#include "Bron/Graphics/VertexArray.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace bron {
class OpenGLRendererAPI : public API {
	static inline int ToOpenGLDrawType(const DrawType type) {
		switch (type) {
			case DrawType::kTriangles:
				return GL_TRIANGLES;
				break;
			case DrawType::kLines:
				return GL_LINES;
				break;
			default:
				BR_CORE_ASSERT(false, "RENDERER: invalid draw type");
		}
		return 0;
	}

	static inline GLint ToOpenGLStencilFunction(const StencilFunction func) {
		switch (func) {
			case StencilFunction::kAlways:
				return GL_ALWAYS;
				break;
			case StencilFunction::kNotEqual:
				return GL_NOTEQUAL;
				break;
		}
		BR_CORE_ASSERT(false, "RENDERER: invalid StencilFunction");
		return 0;
	}

public:
	OpenGLRendererAPI();
	void Init() override;
	void SetClearColor(const glm::vec4& color) override;
	void Clear() override;
	void OnResize(float x, float y, float width, float height) override;

	void EnableBlend() override;
	void EnableDepth() override;
	void DisableDepth() override;

	void SetStencil(StencilFunction func, u32 reference, u32 write_mask) override;
	void DisableStencil() override;

	void SetColorWrite(bool enabled) override;

	void DrawIndexed(const Ref<VertexArray>& v_array, u32 count) override;
	void DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count) override;
	void DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) override;

private:
	void SetDepthTest(bool enabled);

	// Mirrors the GL defaults, so the first call of each actually reaches the driver.
	bool blend_enabled_ = false;
	bool depth_enabled_ = false;
	bool stencil_enabled_ = false;
};
} // namespace bron
