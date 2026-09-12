#include "Command.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace bron {
void Command::Init() { renderer_api_->Init(); }

API* Command::renderer_api_ = new OpenGLRendererAPI();


void Command::ClearColor(const glm::vec4& color) { renderer_api_->SetClearColor(color); }

void Command::Clear() { renderer_api_->Clear(); }

void Command::EnableBlend() { renderer_api_->EnableBlend(); }

void Command::EnableDepth() { renderer_api_->EnableDepth(); }

void Command::DisableDepth() { renderer_api_->DisableDepth(); }

void Command::SetStencil(API::StencilFunction func, u32 reference, u32 write_mask) {
	renderer_api_->SetStencil(func, reference, write_mask);
}

void Command::DisableStencil() { renderer_api_->DisableStencil(); }

void Command::SetColorWrite(const bool enabled) { renderer_api_->SetColorWrite(enabled); }

void Command::DrawIndexed(const Ref<VertexArray>& v_array, u32 count) { renderer_api_->DrawIndexed(v_array, count); }

void Command::DrawIndexedLines(const Ref<VertexArray>& v_array, u32 count) {
	renderer_api_->DrawIndexedLines(v_array, count);
}

void Command::DrawIndexedStripLines(const Ref<VertexArray>& v_array, u32 count) {
	renderer_api_->DrawIndexedStripLines(v_array, count);
}

void Command::OnResize(float x, float y, float width, float height) { renderer_api_->OnResize(x, y, width, height); }
} // namespace bron
