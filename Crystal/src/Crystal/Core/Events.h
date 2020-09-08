#pragma once
//This Event Dispatching System Inspired By https://github.com/TheCherno/Hazel

class Event
{
};

class KeyEvent : public Event
{
public:
	using KeyCode = WPARAM;
	enum class KeyState { Pressed, Released };

	KeyEvent(KeyCode keyCode, KeyState keyState) {}

	KeyState State;
	KeyCode Key;
};

class EventDispatcher
{
public:
	EventDispatcher(Event& event) : m_Event(event) {}

	
	void Dispatch()
	{
		
	}

private:
	Event& m_Event;
};