#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Layers/Layer.h"

#include <vector>

namespace bron {
/// The application's layers in draw order, with one split in the middle:
///
///     [ layer0, layer1 | overlay0, overlay1 ]
///                      ^ insert_index_
///
/// An overlay is not a kind of layer, it is a position: PushLayer inserts before the
/// split, PushOverlay after it. The halves differ only in which end they live at, and
/// that end is what the two iteration directions mean.
///
///   - Updating and rendering runs front to back, so overlays draw on top.
///   - Events run back to front, so overlays get first refusal on the input.
///
/// The same order, reversed - which is the point: whatever is drawn on top is whatever
/// gets clicked. An overlay that marks an event handled keeps it from everything it is
/// covering, so a pause menu can sit over a game without the game seeing the clicks
/// that land on it.
///
/// The stack does not own its layers. They outlive it either way - the editor holds its
/// layer as a member, the Application holds the ImGui one - so this only orders them.
class BR_API LayerStack {
public:
	/// Attaches and inserts before the split.
	void PushLayer(Layer* layer);

	/// Attaches and inserts after it, on top of everything already pushed.
	void PushOverlay(Layer* overlay);

	/// Detaches and removes. A layer that is not in its half of the stack is ignored.
	void PopLayer(Layer* layer);
	void PopOverlay(Layer* overlay);

	/// Detaches everything, last pushed first. Call this while the window and its
	/// graphics context are still up: OnDetach is where a layer releases what it holds
	/// on the GPU, and after the context has gone that is a call into nothing.
	void DetachAll();

	// Draw order. Reverse these for event order.
	auto begin() { return layers_.begin(); }
	auto end() { return layers_.end(); }
	auto rbegin() { return layers_.rbegin(); }
	auto rend() { return layers_.rend(); }

private:
	std::vector<Layer*> layers_;

	/// Where the layer half ends and the overlay half begins.
	u32 insert_index_ = 0;
};
} // namespace bron
