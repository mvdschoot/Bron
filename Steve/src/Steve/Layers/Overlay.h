#ifndef OVERLAY_HEADER
#define OVERLAY_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Layer.h"

#include <vector>
#include <memory>

namespace Steve
{
	class STEVE_API Overlay
	{
	public:
		Overlay(): _visible(true)
		{
		};

		virtual ~Overlay()
		{
		};

		void insertLayer(Layer* layer);
		void insertLayer(Layer* layer, u32 pos);

		void setVisibility(bool vis) { _visible = vis; }
		std::vector<Layer*> getLayers() { return _layer_stack; }
		std::vector<Layer*>::iterator getBegin() { return _layer_stack.begin(); }
		std::vector<Layer*>::iterator getEnd() { return _layer_stack.end(); }

	private:
		std::vector<Layer*> _layer_stack;
		bool _visible;
	};
}


#endif
