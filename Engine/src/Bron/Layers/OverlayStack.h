#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Layers/Overlay.h"

#include <vector>
#include <memory>

namespace bron {
class BR_API OverlayStack {
public:
	OverlayStack();
	~OverlayStack();

	void InsertOverlay(Overlay* overlay);
	void InsertOverlay(Overlay* overlay, u32 pos);

	std::vector<Overlay*> GetOverlays() { return overlay_stack_; };
	std::vector<Overlay*>::iterator GetBegin() { return overlay_stack_.begin(); };
	std::vector<Overlay*>::iterator GetEnd() { return overlay_stack_.end(); };

private:
	std::vector<Overlay*> overlay_stack_;
};
} // namespace bron
