#pragma once

#include "PauseWindow.h"
#include "Button.h"
#include "Window.h"

class BaseFrame;

class PauseWindowMobile : public PauseWindow
{
	Window onScreenButtonsRect;
	Button menuButton;
	Button firingModeButton;

public:
	PauseWindowMobile();
	~PauseWindowMobile(void);
};

