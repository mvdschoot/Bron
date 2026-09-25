//
// Created by mathi on 25-9-2026.
//


#pragma once
#include "Bron/Input/Input.h"

namespace bron {
class ScriptInput : InputSource {
public:
	bool IsKeyPressed(const KeyCode key) const override;
	bool IsMousePressed(const MouseCode key) const override;

	bool receive_keyboard_input = true;
	bool receive_mouse_input = true;
};
} // namespace bron
