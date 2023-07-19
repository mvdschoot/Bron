#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

namespace Steve::graphics
{
	struct FramebufferSpecification
	{
		u32 width;
		u32 height;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		Framebuffer()
		{
		};

		virtual ~Framebuffer()
		{
		};
		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void invalidate() = 0;
		virtual u32 getColorAttachID() = 0;
		virtual u32 getDepthStencilAttachID() = 0;

		static Ref<Framebuffer> Create(FramebufferSpecification& spec);
	};
}

#endif // __FRAMEBUFFER_H__
