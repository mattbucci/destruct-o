#pragma once

#include "Window.h"
#include "Button.h"
#include "Label.h"
#include "ComboBox.h"

class Structure;

class NewStructure : public Window {
	ComboBox sizeBox;
	ComboBox heightBox;
	Button ok;
	Button cancel;
	Label sizeLabel;
	Label heightLabel;
public:
	NewStructure(function<void(Structure*)> onCloseWindow);
	~NewStructure();

	//Show the new structure window
	static void ShowNewStructure(function<void(Structure*)> onCloseWindow);

	//When the user closes or finishes this event fires
	//if the user clicked ok the new structure is placed in Structure*
	//else it is NULL
	GameEvent<void(Structure*)> EventClosedWindow;
};