#include "Overlay.h"

namespace bron {
void Overlay::InsertLayer(Layer* layer) {
	layer->OnAttach();
	layer_stack_.push_back(layer);
}

void Overlay::InsertLayer(Layer* layer, u32 pos) {
	layer->OnAttach();
	layer_stack_.insert(layer_stack_.begin() + pos, layer);
}
} // namespace bron
