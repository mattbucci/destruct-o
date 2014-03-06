#pragma once

class InputEvent {
public:
	enum EventType {
		KeyboardUp,
		KeyboardDown,
		MouseUp,
		MouseDown,
		MouseMove,
		MouseScroll
	};

	//mouse events
	//For scroll events they key is 0 for up, 1 for down
	//At is the time the event occurred
	InputEvent(EventType event,double at, float mouseX, float mouseY, int key = 0);
	//keyboard events
	//At is the time the event occurred
	InputEvent(EventType event,double at, int key);

	double at;
	EventType Event;
	//Used in mouse events
	float MouseX;
	float MouseY;
	//Used in MOUSE_MOVE only!!!
	float RelX;
	float RelY;
    
	//Used in key events (freaking SDL defines fingers as a 64 bit signed type)
	Sint64 Key;
};