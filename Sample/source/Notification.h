#pragma once

#include "Window.h"
#include "Label.h"

class Notification : public Window
{
	Label* text;

	Notification(void);
	~Notification(void);

public:
	static Notification* Singleton;

	static Notification* init();
	void notify(string msg);
	void Draw(GL2DProgram * shader) override;
};