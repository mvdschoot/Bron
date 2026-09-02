#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/Framebuffer.h"

#include "glad/glad.h"

namespace Bron
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(FramebufferSpecification& spec);
		~OpenGLFramebuffer() override;

		void bind() override;
		void unbind() override;
		void invalidate() override;
		u32 getColorAttachID() override;
		u32 getDepthStencilAttachID() override;

	private:
		// Deletes the framebuffer and its attachments. Safe to call on a half-built or
		// already-destroyed framebuffer.
		void destroy();

		// Zero-initialised so the first invalidate() can delete unconditionally - GL
		// silently ignores name 0.
		u32 _renderer_id = 0;
		u32 _color_attachment = 0;
		u32 _depth_stencil_attachment = 0;
		FramebufferSpecification& _spec;
	};
}
