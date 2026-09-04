#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/KeyCodes.h"
#include "Bron/Core/MouseCodes.h"

namespace bron
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMousePressed(MouseCode code);
	};
} // namespace bron
