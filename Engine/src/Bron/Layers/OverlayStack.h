#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Layers/Overlay.h"

#include <vector>
#include <memory>

namespace Bron
{
	class BR_API OverlayStack
	{
	public:
		OverlayStack();
		~OverlayStack();

		void instertOverlay(Overlay* overlay);
		void instertOverlay(Overlay* overlay, u32 pos);

		std::vector<Overlay*> getOverlays() { return _overlay_stack; };
		std::vector<Overlay*>::iterator getBegin() { return _overlay_stack.begin(); };
		std::vector<Overlay*>::iterator getEnd() { return _overlay_stack.end(); };

	private:
		std::vector<Overlay*> _overlay_stack;
	};
}
