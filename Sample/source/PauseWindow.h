#pragma once

#include "Window.h"
#include "Button.h"
#include "Slider.h"

class BaseFrame;

class PauseWindow : public Window
{
	int w, h;
protected:
	BaseFrame* p;

	//Main Menu Elements
	Window menuRect;
	Button menuSave;
	Button menuLoad;
	Button menuOptions;
	Button menuSaveAndQuit;
	Button menuExit;

	//Options Menu Elements
	Window optsRect;
	Label optsViewDistanceLabel;
	Slider<int> optsViewDistance;
	Button optsClose;

	PauseWindow(void);
public:
	PauseWindow(BaseFrame* parent);
	~PauseWindow(void);

	bool toggle();

	virtual void Draw(GL2DProgram * shaders) override;

private:
	void showOptsMenu();
	void hideOptsMenu();
	void resizeElement();
};

