#include "Window.h"

#include "Steve/Core/Platform.h"
#include "Platform/Linux/LinuxWindow.h"

namespace Steve
{
	Ref<Window> Window::Create(const WindowProps& w_props)
	{
		switch (Platform::getOS())
		{
		case Platform::OS::None: CORE_ASSERT(false, "No OS selected!");
		case Platform::OS::Windows: return createRef<LinuxWindow>(w_props);
		case Platform::OS::Linux: return createRef<LinuxWindow>(w_props);
		}
		return nullptr;
	}
}
