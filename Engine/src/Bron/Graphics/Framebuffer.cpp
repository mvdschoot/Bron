#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Bron/Core/Platform.h"

namespace bron
{
	Ref<Framebuffer> Framebuffer::Create(FramebufferSpecification& spec)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::None: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}
		return nullptr;
	}
} // namespace bron
