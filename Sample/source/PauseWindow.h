#pragma once

#include "Window.h"
#include "Button.h"

class BaseFrame;

class PauseWindow : public Window
{
protected:
	BaseFrame* parent;

	//Main Menu Elements
	Window menuRect;
	Button save;
	Button load;
	Button options;
	Button saveandquit;

	//Options Menu Elements
	Window optsRect;
	Button optsViewdistanceUp;
	Button optsViewdistanceDown;
	Button optsClose;

	PauseWindow(void);
public:
	PauseWindow(BaseFrame* parent);
	~PauseWindow(void);

	bool toggle();

private:
	void showOptsMenu();
	void hideOptsMenu();
};

