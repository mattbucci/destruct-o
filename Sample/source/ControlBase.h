#pragma once

#include "Control.h"
#include "InputEvent.h"
#include "DebugWindow.h"

class Window;
class Label;

class ControlBase : Control {
	set<Control*> toDelete;
	void DestroyRequestedObjects();
	Sint64 pressedKey;
	//lastKeyProcessed determines when a key event was last sent
	double lastKeyProcessedAt;
	//First key press is special, it has a long gap after it
	bool firstKeyPressRecorded;

public:
	ControlBase();

	//Add a window to the list of active controls
	void AddWindow(Window * w);

	//Pass events to the active windows
	//returns whether or not the event was consumed by the window system
	bool FireEvent(InputEvent event);

	//Schedule a control for destruction upon next time an event ends
	//controls scheduled multiple times are automatically ignored
	void RequestControlDestroyed(Control * toDestroy);

	//Used for showing debug info in the upper left
	DebugWindow Debug;

	//Draw all contrls
	void Draw(GL2DProgram * prg, Rect size);
};