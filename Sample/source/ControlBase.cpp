#include "stdafx.h"
#include "ControlBase.h"
#include "Window.h"
#include "OS.h"
#include "Label.h"
#include <sstream>

ControlBase::ControlBase() {
	color = vec4(0,0,0,0);
	pressedKey = -1;
	AddChild(&Debug);
}

void ControlBase::DestroyRequestedObjects() {
	for (auto & req : toDelete) {
		delete req;
	}
	toDelete.clear();
}

void ControlBase::AddWindow(Window * w) {
	Control::AddChild(w);
}

bool ControlBase::FireEvent(InputEvent event) {
	bool eventConsumed = false;
	//Pass an event received from the event system to the control (can be passed to lua)
	if (event.Event == InputEvent::MouseDown) {
		Control::OnMousePress(vec2(event.MouseX,event.MouseY),event.Key,true);
		eventConsumed = hasFocusedChild();
	}
		
	else if (event.Event == InputEvent::MouseUp) {
		Control::OnMousePress(vec2(event.MouseX,event.MouseY),event.Key,false);
		eventConsumed = hasFocusedChild();
	}
	else if (event.Event == InputEvent::MouseMove) {
		Control::OnMouseMove(vec2(event.MouseX,event.MouseY));
		eventConsumed = hasFocusedChild();
	}
	else if (event.Event == InputEvent::MouseScroll) {
		Control::OnMouseScroll(event.Key>0);
		eventConsumed = hasFocusedChild();
	}
	//Process key events pending
	if (event.Event == InputEvent::KeyboardUp) {
		eventConsumed = (keyboardFocusedChild != NULL);

		//Key released
		if (event.Key == pressedKey)
			pressedKey = -1;
	}
	else if (event.Event == InputEvent::KeyboardDown) {
		eventConsumed = (keyboardFocusedChild != NULL);

		lastKeyProcessedAt = OS::Now()-.1; //should probably use the event time
		pressedKey = event.Key;
		firstKeyPressRecorded = false;
	}
	//Cleanup any objects which requested their destruction during their events
	DestroyRequestedObjects();

	return eventConsumed;
}

void ControlBase::Draw(GL2DProgram * prg, Rect size) {
	//During draw events update key events
	if ((pressedKey != -1) && ((OS::Now() - lastKeyProcessedAt)) >= .1) {
		//The first key press has a much longer gap before its repeated
		if (!firstKeyPressRecorded) {
			lastKeyProcessedAt += .6;
			firstKeyPressRecorded = true;
		}
		else
			lastKeyProcessedAt += .1;
		if (keyboardFocusedChild != NULL)
			keyboardFocusedChild->__OnKeyPress(pressedKey);
	}
	//Send width,height,Control::Draw()
	this->position.Width = size.Width;
	this->position.Height = size.Height;

	Control::Draw(prg);
}

void ControlBase::RequestControlDestroyed(Control * toDestroy) {
	if (toDelete.find(toDestroy) == toDelete.end())
		toDelete.insert(toDestroy);
}

