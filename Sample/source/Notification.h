#pragma once

#include "Window.h"
#include "Label.h"

class Notification : public Window
{
	Label* text;

public:
	Notification(void);
	~Notification(void);

	void notify(string msg);
	void Draw(GL2DProgram * shader) override;
};