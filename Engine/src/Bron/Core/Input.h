#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/KeyCodes.h"
#include "Bron/Core/MouseCodes.h"

namespace Bron
{
	class Input
	{
	public:
		static bool isKeyPressed(KeyCode key);
		static bool isMousePressed(MouseCode code);
	};
} // namespace Bron
