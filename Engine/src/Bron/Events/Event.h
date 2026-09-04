#pragma once

#include "Bron/Core/Core.h"

#include <ostream>
#include <functional>

namespace bron
{
#define BR_EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const { return GetStaticType(); }\
								virtual const char* GetName() const { return #type; }

#define BR_EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const { return category; }

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
		EventCategoryApplication = BR_BIT(0),
		EventCategoryInput = BR_BIT(1),
		EventCategoryKeyboard = BR_BIT(2),
		EventCategoryMouseButton = BR_BIT(3),
		EventCategoryMouse = BR_BIT(4)
	};

	class BR_API Event
	{
	public:
		static EventType GetStaticType();
		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual const char* GetName() const = 0;

		bool IsInCategory(EventCategory cat)
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
			event_(evt)
		{
		};

		~EventDispatcher()
		{
		};

		//
		template <typename T>
		bool Dispatch(EventFn<T> fn)
		{
			if (event_.GetEventType() == T::GetStaticType())
			{
				event_.is_handled = fn(*static_cast<T*>(&event_));
				return true;
			}
			return false;
		}

	private:
		Event& event_;
	};

	// inline std::ostream& operator<<(std::ostream& os, const Event& e)
	// {
	// 	return os << e.GetName();
	// }
}
