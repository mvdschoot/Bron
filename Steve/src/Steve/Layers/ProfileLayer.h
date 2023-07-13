#ifndef __PROFILELAYER_H__
#define __PROFILELAYER_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Layers/Layer.h"

#include <imgui.h>

#include <map>

namespace Steve
{
	class ProfileLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
	};
}
#endif // __PROFILELAYER_H__
