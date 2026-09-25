//
// Created by mathi on 25-9-2026.
//

#include "PanelInput.h"
#include "Panel.h"

namespace bron::editor {
bool PanelInput::IsKeyPressed(const KeyCode key) const { return panel_->IsFocussed() && Input::IsKeyPressed(key); }
bool PanelInput::IsMousePressed(const MouseCode key) const { return panel_->IsHovered() && Input::IsMousePressed(key); }
} // namespace bron::editor
