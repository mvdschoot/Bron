#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron
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
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Invalidate() = 0;
		virtual u32 GetColorAttachId() = 0;
		virtual u32 GetDepthStencilAttachId() = 0;

		static Ref<Framebuffer> Create(FramebufferSpecification& spec);
	};
}
