
#pragma once

#include "Control.h"
#include "Shape.h"
#include "RasterizedText.h"
#include "GameEventSubscriber.h"

class Window : public Control, protected GameEventSubscriber {

	//An optional automatically placed title
	RasterizedText title;
public:
	Window();
	Window(string title);
	Window(Rect position, string title);
	virtual ~Window();

	//Add a control as a child of this window
	void AddControl(Control * controlToAdd);

	//Set the title text
	void SetTitle(string text);
	string GetTitle();

	//If this window is on top and returns true, no other windows will receive input
	//defaults to false
	virtual bool IsDialog();

	virtual void Draw(GL2DProgram * shaders) override;
};
