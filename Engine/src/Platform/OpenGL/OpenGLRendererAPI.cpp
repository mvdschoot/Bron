#include "OpenGLRendererAPI.h"

namespace bron {
OpenGLRendererAPI::OpenGLRendererAPI() {}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
	BR_PROFILE_FUNCTION();
	glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear() {
	BR_PROFILE_FUNCTION();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererAPI::OnResize(float x, float y, float width, float height) {
	BR_PROFILE_FUNCTION();
	glViewport(x, y, width, height);
}

// Blending and depth are tracked apart from each other so an overlay pass can drop the
// depth test on its own. The two Enable* calls keep their old meaning: each one puts the
// pipeline fully into 2D or 3D mode.
void OpenGLRendererAPI::SetDepthTest(const bool enabled) {
	if (depth_enabled_ == enabled)
		return;
	depth_enabled_ = enabled;

	if (enabled) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void OpenGLRendererAPI::EnableBlend() {
	if (!blend_enabled_) {
		blend_enabled_ = true;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	SetDepthTest(false);
}

void OpenGLRendererAPI::EnableDepth() {
	if (blend_enabled_) {
		blend_enabled_ = false;
		glDisable(GL_BLEND);
	}

	SetDepthTest(true);
}

void OpenGLRendererAPI::DisableDepth() { SetDepthTest(false); }

void OpenGLRendererAPI::SetStencil(const StencilFunction func, const u32 reference, const u32 write_mask) {
	if (!stencil_enabled_) {
		stencil_enabled_ = true;
		glEnable(GL_STENCIL_TEST);
	}

	// The compare mask stays wide open - only the write mask is worth varying, and it is
	// what separates a pass that marks the buffer from one that merely reads it.
	glStencilFunc(ToOpenGLStencilFunction(func), reference, 0xff);

	// Fragments that fail the depth test still mark the stencil. Without that, the part of
	// an object hidden behind other geometry leaves no mark, and a following pass keyed on
	// "not marked" fills it in solid instead of leaving it alone.
	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	glStencilMask(write_mask);
}

void OpenGLRendererAPI::DisableStencil() {
	if (!stencil_enabled_)
		return;
	stencil_enabled_ = false;

	glDisable(GL_STENCIL_TEST);
	// glClear obeys the write mask, so leaving it closed would quietly stop the stencil
	// buffer from ever being cleared again.
	glStencilMask(0xff);
}

void OpenGLRendererAPI::SetColorWrite(const bool enabled) {
	const GLboolean write = enabled ? GL_TRUE : GL_FALSE;
	glColorMask(write, write, write, write);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& v_array, u32 count) {
	BR_PROFILE_FUNCTION();

	v_array->Bind();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRendererAPI::DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count) {
	BR_PROFILE_FUNCTION();

	u32 c = count ? count : v_array->GetIndexBuffer()->GetCount();
	v_array->Bind();
	glDrawElements(GL_LINES, c, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRendererAPI::DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) {
	BR_PROFILE_FUNCTION();

	u32 c = count ? count : v_array->GetIndexBuffer()->GetCount();
	v_array->Bind();
	glDrawElements(GL_LINE_STRIP, c, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRendererAPI::Init() { BR_PROFILE_FUNCTION(); }
} // namespace bron
