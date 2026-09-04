#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/Framebuffer.h"

#include "glad/glad.h"

namespace bron
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(FramebufferSpecification& spec);
		~OpenGLFramebuffer() override;

		void Bind() override;
		void Unbind() override;
		void Invalidate() override;
		u32 GetColorAttachId() override;
		u32 GetDepthStencilAttachId() override;

	private:
		// Deletes the framebuffer and its attachments. Safe to call on a half-built or
		// already-destroyed framebuffer.
		void Destroy();

		// Zero-initialised so the first Invalidate() can delete unconditionally - GL
		// silently ignores name 0.
		u32 renderer_id_ = 0;
		u32 color_attachment_ = 0;
		u32 depth_stencil_attachment_ = 0;
		FramebufferSpecification& spec_;
	};
}
