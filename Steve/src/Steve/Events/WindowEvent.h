#ifndef WINDOW_EVENT_HEADER
#define WINDOW_EVENT_HEADER

#include "Steve/Events/Event.h"

namespace Steve
{
	class STEVE_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) :
			_width(width), _height(height)
		{
		};


		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowResize);

		int getWidth() { return _width; }
		int getHeight() { return _height; }

	private:
		int _width, _height;
	};

	class STEVE_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowClose);

	private:
	};

	class STEVE_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent()
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowFocus);

	private:
	};

	class STEVE_API WindowUnfocusEvent : public Event
	{
	public:
		WindowUnfocusEvent()
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowUnfocus);

	private:
	};
}

#endif
