//
// Created by mathi on 16-9-2026.
//


#pragma once
#include "Bron/Layers/Layer.h"

namespace bron {
class WorldLayer : public Layer {
	void OnAttach() override;
	void OnDetach() override;
	void OnEvent(Event& event) override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;
};
} // namespace bron
