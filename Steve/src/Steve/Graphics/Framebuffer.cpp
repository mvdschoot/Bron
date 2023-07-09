#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Steve/Core/Platform.h"

namespace Steve::graphics
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<graphics::OpenGLFramebuffer>(spec);
		}
		return nullptr;
	}
} // namespace Steve
