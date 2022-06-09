#ifndef __APP_LAYER_HEADER__
#define __APP_LAYER_HEADER__

#include "Cheets.h"

namespace Cheets
{
	class AppLayer : public Layer
	{
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
	};
}

#endif
