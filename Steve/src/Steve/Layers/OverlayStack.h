#ifndef OVERLAYSTACK_HEADER
#define OVERLAYSTACK_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Layers/Overlay.h"

#include <vector>
#include <memory>

namespace Steve
{
	class STEVE_API OverlayStack
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


#endif
