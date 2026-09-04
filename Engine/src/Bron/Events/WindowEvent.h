#pragma once

#include "Bron/Events/Event.h"

namespace bron {
class BR_API WindowResizeEvent : public Event {
public:
	WindowResizeEvent(unsigned int width, unsigned int height) : width_(width), height_(height) {};


	BR_EVENT_CLASS_CATEGORY(EventCategory::kApplication);
	BR_EVENT_CLASS_TYPE(WindowResize);

	int GetWidth() { return width_; }
	int GetHeight() { return height_; }

private:
	int width_, height_;
};

class BR_API WindowCloseEvent : public Event {
public:
	WindowCloseEvent() {};

	BR_EVENT_CLASS_CATEGORY(EventCategory::kApplication);
	BR_EVENT_CLASS_TYPE(WindowClose);

private:
};

class BR_API WindowFocusEvent : public Event {
public:
	WindowFocusEvent() {};

	BR_EVENT_CLASS_CATEGORY(EventCategory::kApplication);
	BR_EVENT_CLASS_TYPE(WindowFocus);

private:
};

class BR_API WindowUnfocusEvent : public Event {
public:
	WindowUnfocusEvent() {};

	BR_EVENT_CLASS_CATEGORY(EventCategory::kApplication);
	BR_EVENT_CLASS_TYPE(WindowUnfocus);

private:
};
} // namespace bron
