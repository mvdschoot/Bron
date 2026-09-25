//
// Created by mathi on 25-9-2026.
//

#include "ScriptInput.h"

namespace bron {
bool ScriptInput::IsKeyPressed(const KeyCode key) const {
	return receive_keyboard_input && InputSource::IsKeyPressed(key);
}
bool ScriptInput::IsMousePressed(const MouseCode key) const {
	return receive_mouse_input && InputSource::IsMousePressed(key);
}
} // namespace bron
