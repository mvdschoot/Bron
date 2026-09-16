#include "LayerStack.h"

#include <algorithm>

namespace bron {
void LayerStack::PushLayer(Layer* layer) {
	// Inserted before OnAttach runs, so a layer that pushes another one from its own
	// OnAttach still lands behind it.
	layers_.insert(layers_.begin() + insert_index_, layer);
	insert_index_++;

	layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay) {
	layers_.push_back(overlay);
	overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer) {
	const auto split = layers_.begin() + insert_index_;

	const auto it = std::find(layers_.begin(), split, layer);
	if (it == split)
		return;

	layer->OnDetach();
	layers_.erase(it);
	insert_index_--;
}

void LayerStack::PopOverlay(Layer* overlay) {
	const auto it = std::find(layers_.begin() + insert_index_, layers_.end(), overlay);
	if (it == layers_.end())
		return;

	overlay->OnDetach();
	layers_.erase(it);
}

void LayerStack::DetachAll() {
	// Last pushed first: a layer put on later is the one more likely to be holding
	// something an earlier one set up.
	for (auto it = layers_.rbegin(); it != layers_.rend(); ++it)
		(*it)->OnDetach();

	// Cleared so calling this twice - or pushing again afterwards - is not a surprise.
	layers_.clear();
	insert_index_ = 0;
}
} // namespace bron
