#pragma once

#include "Bron/Core/Core.h"

#include <ostream>
#include <functional>

namespace Bron
{
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const { return GetStaticType(); }\
								virtual const char* GetName() const { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const { return category; }

	enum class EventType
	{
		None = 0,
		KeyPressed,
		KeyReleased,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowUnfocus,
		MouseMoved,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouseButton = BIT(3),
		EventCategoryMouse = BIT(4)
	};

	class BR_API Event
	{
	public:
		static EventType GetStaticType();
		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual const char* GetName() const = 0;

		bool isInCategory(EventCategory cat)
		{
			return GetCategoryFlags() & cat;
		}

		bool is_handled = false;

	private:
		//
	};


	class EventDispatcher
	{
		template <typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& evt) :
			_event(evt)
		{
		};

		~EventDispatcher()
		{
		};

		//
		template <typename T>
		bool Dispatch(EventFn<T> fn)
		{
			if (_event.GetEventType() == T::GetStaticType())
			{
				_event.is_handled = fn(*static_cast<T*>(&_event));
				return true;
			}
			return false;
		}

	private:
		Event& _event;
	};

	// inline std::ostream& operator<<(std::ostream& os, const Event& e)
	// {
	// 	return os << e.GetName();
	// }
}
