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
	Button menuExitButton;

	PauseWindowMobile(void);
public:
	PauseWindowMobile(BaseFrame* parent);
	~PauseWindowMobile(void);
};

