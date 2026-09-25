//
// Created by mathi on 25-9-2026.
//

#pragma once
#include "Bron/Input/Input.h"

namespace bron::editor {
class Panel;
class PanelInput : InputSource {
public:
	explicit PanelInput(Panel* panel) : panel_(panel) {}

	bool IsKeyPressed(const KeyCode key) const override;
	bool IsMousePressed(const MouseCode key) const override;

private:
	Panel* panel_;
};

} // namespace bron::editor
