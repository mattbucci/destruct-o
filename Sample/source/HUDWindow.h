#pragma once

#include "Window.h"
#include "Button.h"

class BaseFrame;

class HUDWindow : public Window
{
	BaseFrame* parent;

	HUDWindow(void);
public:
	HUDWindow(BaseFrame* parent);
	~HUDWindow(void);

	virtual void Draw(GL2DProgram * shaders) override;
};

