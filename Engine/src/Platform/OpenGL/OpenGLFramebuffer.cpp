#include "OpenGLFramebuffer.h"

namespace bron
{
	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpecification& spec)
		: spec_(spec)
	{
		OpenGLFramebuffer::Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Destroy();
	}

	// The attachments are owned by this framebuffer, so they go with it - deleting only the
	// framebuffer object leaves the two textures alive.
	void OpenGLFramebuffer::Destroy()
	{
		glDeleteFramebuffers(1, &renderer_id_);
		glDeleteTextures(1, &color_attachment_);
		glDeleteTextures(1, &depth_stencil_attachment_);

		renderer_id_ = 0;
		color_attachment_ = 0;
		depth_stencil_attachment_ = 0;
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
		glViewport(0, 0, spec_.width, spec_.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		// Invalidate() is a resize as much as a first-time build, so anything from a previous
		// call has to go first, otherwise every viewport resize leaks a framebuffer and two
		// textures.
		Destroy();

		glCreateFramebuffers(1, &renderer_id_);
		glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

		glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
		glBindTexture(GL_TEXTURE_2D, color_attachment_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec_.width,
		             spec_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &depth_stencil_attachment_);
		glBindTexture(GL_TEXTURE_2D, depth_stencil_attachment_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, spec_.width,
		             spec_.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_attachment_,
		                       0);

		BR_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
	}

	u32 OpenGLFramebuffer::GetColorAttachId()
	{
		return color_attachment_;
	}

	u32 OpenGLFramebuffer::GetDepthStencilAttachId()
	{
		return depth_stencil_attachment_;
	}
}
