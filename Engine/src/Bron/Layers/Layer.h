#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Events/Event.h"
#include "Bron/Core/Timestep.h"

#include <string>

namespace bron {
class BR_API Layer {
public:
	Layer() {}

	virtual ~Layer() = default;

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnEvent(Event& event) = 0;
	virtual void OnUpdate(Timestep ts) = 0;
	virtual void OnImGuiRender() = 0;

	// inline const std::string& GetName() {return debug_name_;};
};
} // namespace bron
