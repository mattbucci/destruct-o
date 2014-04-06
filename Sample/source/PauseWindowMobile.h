#pragma once

#include "PauseWindow.h"
#include "Button.h"

class BaseFrame;

class PauseWindowMobile : public PauseWindow
{
	PauseWindowMobile(void);
public:
	PauseWindowMobile(BaseFrame* parent);
	~PauseWindowMobile(void);
};

