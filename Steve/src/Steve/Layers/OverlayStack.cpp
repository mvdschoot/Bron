#include "OverlayStack.h"

namespace Steve
{
	OverlayStack::OverlayStack()
	{
	}

	OverlayStack::~OverlayStack()
	{
	}

	void OverlayStack::instertOverlay(Overlay* overlay)
	{
		_overlay_stack.push_back(overlay);
	}

	void OverlayStack::instertOverlay(Overlay* overlay, u32 pos)
	{
		_overlay_stack.insert(_overlay_stack.begin() + pos, overlay);
	}
}
