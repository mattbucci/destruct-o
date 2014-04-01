#pragma once

#include "Window.h"
#include "Button.h"

class PauseWindow : public Window
{
	Window menuRect;

	Button save;
	Button load;
	Button options;
	Button saveandquit;
public:
	PauseWindow(void);
	~PauseWindow(void);
};

