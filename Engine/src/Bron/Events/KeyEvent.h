#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Events/Event.h"
#include "Bron/Core/KeyCodes.h"

namespace Bron
{
	class BR_API KeyEvent : public Event
	{
	public:
		KeyEvent(KeyCode key) : _key_code(key)
		{
		};

		virtual KeyCode getKey()
		{
			return _key_code;
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput);

	protected:
		KeyCode _key_code;
	};


	class BR_API KeyPressedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		EVENT_CLASS_TYPE(KeyPressed);

	private:
	};

	class BR_API KeyReleasedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		EVENT_CLASS_TYPE(KeyReleased);

	private:
	};
}
