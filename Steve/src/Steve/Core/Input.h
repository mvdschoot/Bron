#ifndef __INPUT_H__
#define __INPUT_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/KeyCodes.h"
#include "Steve/Core/MouseCodes.h"

namespace Steve
{
	class Input
	{
	public:
		static bool isKeyPressed(KeyCode key);
		static bool isMousePressed(MouseCode code);
	};
} // namespace Steve


#endif // __INPUT_H__
