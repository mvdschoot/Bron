#include "OpenGLFramebuffer.h"

namespace Steve::graphics
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: _spec(spec)
	{
		invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &_renderer_id);
	}

	void OpenGLFramebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _renderer_id);
		glViewport(0, 0, 1280, 720);
	}

	void OpenGLFramebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::invalidate()
	{
		glCreateFramebuffers(1, &_renderer_id);
		glBindFramebuffer(GL_FRAMEBUFFER, _renderer_id);

		glCreateTextures(GL_TEXTURE_2D, 1, &_color_attachment);
		glBindTexture(GL_TEXTURE_2D, _color_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _spec.width,
		             _spec.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color_attachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &_depth_stencil_attachment);
		glBindTexture(GL_TEXTURE_2D, _depth_stencil_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _spec.width,
		             _spec.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depth_stencil_attachment,
		                       0);

		CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete");
	}

	u32 OpenGLFramebuffer::getColorAttachID()
	{
		return _color_attachment;
	}

	u32 OpenGLFramebuffer::getDepthStencilAttachID()
	{
		return _depth_stencil_attachment;
	}
}
