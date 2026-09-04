#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Events/Event.h"
#include "Bron/Core/KeyCodes.h"

namespace bron
{
	class BR_API KeyEvent : public Event
	{
	public:
		KeyEvent(KeyCode key) : key_code_(key)
		{
		};

		virtual KeyCode GetKey()
		{
			return key_code_;
		};

		BR_EVENT_CLASS_CATEGORY(EventCategory::kMouse | EventCategory::kInput);

	protected:
		KeyCode key_code_;
	};


	class BR_API KeyPressedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		BR_EVENT_CLASS_TYPE(KeyPressed);

	private:
	};

	class BR_API KeyReleasedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		BR_EVENT_CLASS_TYPE(KeyReleased);

	private:
	};
}
