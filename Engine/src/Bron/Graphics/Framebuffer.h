#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace Bron
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
