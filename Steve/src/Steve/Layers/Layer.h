#ifndef LAYER_HEADER
#define LAYER_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Events/Event.h"
#include "Steve/Core/Timestep.h"

#include <string>

namespace Steve
{
	class STEVE_API Layer
	{
	public:
		Layer()
		{
		}

		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnEvent(Event& event) = 0;
		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnImGuiRender() = 0;

		//inline const std::string& GetName() {return _debug_name;};
	};
}


#endif
