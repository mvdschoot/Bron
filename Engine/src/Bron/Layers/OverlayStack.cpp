#include "OverlayStack.h"

namespace bron
{
	OverlayStack::OverlayStack()
	{
	}

	OverlayStack::~OverlayStack()
	{
	}

	void OverlayStack::InsertOverlay(Overlay* overlay)
	{
		overlay_stack_.push_back(overlay);
	}

	void OverlayStack::InsertOverlay(Overlay* overlay, u32 pos)
	{
		overlay_stack_.insert(overlay_stack_.begin() + pos, overlay);
	}
}
