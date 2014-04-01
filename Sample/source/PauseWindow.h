#pragma once

#include "Window.h"
#include "Button.h"

class PauseWindow : public Window
{
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

public:
	PauseWindow(void);
	~PauseWindow(void);

private:
	void showOptsMenu();
	void hideOptsMenu();
};

