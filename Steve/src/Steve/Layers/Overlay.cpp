#include "Overlay.h"

namespace Steve
{
	void Overlay::insertLayer(Layer* layer)
	{
		layer->OnAttach();
		_layer_stack.push_back(layer);
	}

	void Overlay::insertLayer(Layer* layer, u32 pos)
	{
		layer->OnAttach();
		_layer_stack.insert(_layer_stack.begin() + pos, layer);
	}
}
