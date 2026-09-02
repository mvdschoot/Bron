#pragma once

#include "Bron/Events/Event.h"

namespace Bron
{
	class BR_API WindowResizeEvent : public Event
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

	class BR_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowClose);

	private:
	};

	class BR_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent()
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
		EVENT_CLASS_TYPE(WindowFocus);

	private:
	};

	class BR_API WindowUnfocusEvent : public Event
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
