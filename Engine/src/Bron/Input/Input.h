#pragma once

#include "Bron/Core/Core.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

namespace bron {
class Input {
public:
	static bool IsKeyPressed(KeyCode key);
	static bool IsMousePressed(MouseCode code);
	static void SetBlocked(bool blocked);

private:
	static inline bool blocked_ = false;
};

/// Helper class for directing input to the appropriate destination
/// Override the methods.
class InputSource {
public:
	virtual ~InputSource() = default;
	virtual bool IsKeyPressed(const KeyCode key) const { return Input::IsKeyPressed(key); }
	virtual bool IsMousePressed(const MouseCode key) const { return Input::IsMousePressed(key); }
};
} // namespace bron
