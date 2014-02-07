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
	float MouseX;
	float MouseY;
	int Key;
};