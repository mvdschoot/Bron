#ifndef __OPENGLFRAMEBUFFER_H__
#define __OPENGLFRAMEBUFFER_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Steve/Graphics/Framebuffer.h"

#include "glad/glad.h"

namespace Steve
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
		u32 _renderer_id;
		u32 _color_attachment;
		u32 _depth_stencil_attachment;
		FramebufferSpecification& _spec;
	};
}

#endif // __OPENGLFRAMEBUFFER_H__
