#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/Framebuffer.h"

namespace bron {
class OpenGLFramebuffer : public Framebuffer {
public:
	OpenGLFramebuffer(FramebufferSpecification& spec);
	~OpenGLFramebuffer() override;

	void Bind() override;
	void Unbind() override;
	void Invalidate() override;
	u32 GetColorAttachId(u32 index) override;
	u32 GetDepthStencilAttachId() override;

	int ReadPixelInt(u32 index, int position_x, int position_y) override;
	void ClearAttachmentInt(u32 index, int value) override;

private:
	// Deletes the framebuffer and its attachments. Safe to call on a half-built or
	// already-destroyed framebuffer.
	void Destroy();

	// Zero-initialised so the first Invalidate() can delete unconditionally - GL
	// silently ignores name 0.
	u32 renderer_id_ = 0;
	std::vector<u32> color_attachments_;
	/// Format per colour attachment, indexed the same way as color_attachments_ - the spec
	/// list cannot be indexed directly because depth formats live in it too.
	std::vector<FramebufferTextureFormat> color_formats_;
	u32 depth_stencil_attachment_ = 0;
	FramebufferSpecification& spec_;
};
} // namespace bron
