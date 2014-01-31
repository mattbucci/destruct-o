#pragma once

#include "Control.h"
#include "InputEvent.h"

class Window;
class Label;

class ControlBase : Control {
	set<Control*> toDelete;
	void DestroyRequestedObjects();
	int pressedKey;
	//lastKeyProcessed determines when a key event was last sent
	double lastKeyProcessedAt;
	//First key press is special, it has a long gap after it
	bool firstKeyPressRecorded;

	//For debug
	bool debugDisplayOn;
	Label * debugFps;
public:
	ControlBase();

	void DebugEnableDebugDisplay(bool display);
	void DebugSetFPS(float fps);

	//Add a window to the list of active controls
	void AddWindow(Window * w);

	//Pass events to the active windows
	//returns whether or not the event was consumed by the window system
	bool FireEvent(InputEvent event);

	//Schedule a control for destruction upon next time an event ends
	//controls scheduled multiple times are automatically ignored
	void RequestControlDestroyed(Control * toDestroy);



	//Draw all contrls
	void Draw(GL2DProgram * prg, Rect size);
};