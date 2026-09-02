#pragma once

#include "Bron/Events/Event.h"
#include "Bron/Core/KeyCodes.h"
#include "Bron/Core/MouseCodes.h"

namespace Bron
{
	class BR_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y):
			_mouseX(x), _mouseY(y)
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput);
		EVENT_CLASS_TYPE(MouseMoved);

	private:
		float _mouseX, _mouseY;
	};

	class BR_API MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int key):
			_key(static_cast<MouseCode>(key))
		{
		};

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouseButton | EventCategory::EventCategoryInput);

	private:
		MouseCode _key;
	};

	class BR_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		using MouseButtonEvent::MouseButtonEvent;
		EVENT_CLASS_TYPE(MouseButtonPressed);

	private:
	};

	class BR_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		using MouseButtonEvent::MouseButtonEvent;
		EVENT_CLASS_TYPE(MouseButtonReleased);

	private:
	};

	class BR_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float offx, const float offy) :
			_offx(offx), _offy(offy)
		{
		};


		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse || EventCategory::EventCategoryInput);
		EVENT_CLASS_TYPE(MouseScrolled);

		float getOffsetX()
		{
			return _offx;
		}

		float getOffsetY()
		{
			return _offy;
		}

	private:
		float _offx, _offy;
	};
}
