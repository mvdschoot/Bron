#include "Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Bron/Core/Platform.h"

namespace Bron
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
} // namespace Bron
