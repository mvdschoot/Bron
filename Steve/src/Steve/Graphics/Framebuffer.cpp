#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Steve/Core/Platform.h"

namespace Steve
{
	Ref<Framebuffer> Framebuffer::Create(FramebufferSpecification& spec)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLFramebuffer>(spec);
		}
		return nullptr;
	}
} // namespace Steve
