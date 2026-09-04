#include "Window.h"

#include "Platform/Desktop/DesktopWindow.h"

namespace bron
{
	// Every desktop platform we support goes through GLFW, so there is nothing
	// to branch on here. Add a case only when a backend genuinely differs.
	Ref<Window> Window::Create(const WindowProps& w_props)
	{
		return CreateRef<DesktopWindow>(w_props);
	}
}
