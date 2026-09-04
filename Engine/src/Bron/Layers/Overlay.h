#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Layer.h"

#include <vector>
#include <memory>

namespace bron
{
	class BR_API Overlay
	{
	public:
		Overlay(): visible_(true)
		{
		};

		virtual ~Overlay()
		{
		};

		void InsertLayer(Layer* layer);
		void InsertLayer(Layer* layer, u32 pos);

		void SetVisibility(bool vis) { visible_ = vis; }
		std::vector<Layer*> GetLayers() { return layer_stack_; }
		std::vector<Layer*>::iterator GetBegin() { return layer_stack_.begin(); }
		std::vector<Layer*>::iterator GetEnd() { return layer_stack_.end(); }

	private:
		std::vector<Layer*> layer_stack_;
		bool visible_;
	};
}
