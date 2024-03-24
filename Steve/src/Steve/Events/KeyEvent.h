#ifndef KEY_EVENT_HEADER
#define KEY_EVENT_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Events/Event.h"
#include "Steve/Core/KeyCodes.h"

namespace Steve
{
	class STEVE_API KeyEvent : public Event
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


	class STEVE_API KeyPressedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		EVENT_CLASS_TYPE(KeyPressed);

	private:
	};

	class STEVE_API KeyReleasedEvent : public KeyEvent
	{
	public:
		using KeyEvent::KeyEvent;
		EVENT_CLASS_TYPE(KeyReleased);

	private:
	};
}

#endif
