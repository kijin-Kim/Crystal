#pragma once
//This Event Dispatching System Inspired By https://github.com/TheCherno/Hazel

class Event
{
public:
	Event() = default;
	virtual ~Event() = default;
};

class KeyEvent : public Event
{
public:
	using KeyCode = WPARAM;
	enum class KeyState { Pressed, Released };

	KeyEvent(KeyCode keyCode, KeyState keyState) {}

	KeyState State = {};
	KeyCode Key = {};
};

class EventDispatcher
{
public:
	static void Dispatch(Event& e)
	{

	}
};