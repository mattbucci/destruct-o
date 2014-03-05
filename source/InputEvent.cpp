#include "stdafx.h"
#include "InputEvent.h"

//mouse events
InputEvent::InputEvent(EventType event,double at, float mouseX, float mouseY, int key) {
	this->Event = event;
	MouseX = mouseX;
	MouseY = mouseY;
	Key = key;
	this->at = at;
}
//keyboard events
InputEvent::InputEvent(EventType event,double at, int key)  {
	Event = event;
	Key = key;
	this->at = at;
}