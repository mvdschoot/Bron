#pragma once

#include "Bron/Events/Event.h"
#include "Bron/Core/KeyCodes.h"
#include "Bron/Core/MouseCodes.h"

namespace bron
{
	class BR_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y):
			mouse_x_(x), mouse_y_(y)
		{
		};

		BR_EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse | EventCategory::EventCategoryInput);
		BR_EVENT_CLASS_TYPE(MouseMoved);

	private:
		float mouse_x_, mouse_y_;
	};

	class BR_API MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int key):
			key_(static_cast<MouseCode>(key))
		{
		};

		BR_EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouseButton | EventCategory::EventCategoryInput);

	private:
		MouseCode key_;
	};

	class BR_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		using MouseButtonEvent::MouseButtonEvent;
		BR_EVENT_CLASS_TYPE(MouseButtonPressed);

	private:
	};

	class BR_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		using MouseButtonEvent::MouseButtonEvent;
		BR_EVENT_CLASS_TYPE(MouseButtonReleased);

	private:
	};

	class BR_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float offx, const float offy) :
			offx_(offx), offy_(offy)
		{
		};


		BR_EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMouse || EventCategory::EventCategoryInput);
		BR_EVENT_CLASS_TYPE(MouseScrolled);

		float GetOffsetX()
		{
			return offx_;
		}

		float GetOffsetY()
		{
			return offy_;
		}

	private:
		float offx_, offy_;
	};
}
