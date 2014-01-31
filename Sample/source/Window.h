
#pragma once

#include "Control.h"
#include "Shape.h"
#include "RasterizedText.h"

class Window : public Control {

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


	virtual void Draw(GL2DProgram * shaders) override;
};
