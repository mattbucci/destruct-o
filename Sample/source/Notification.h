#pragma once

#include "Window.h"
#include "Label.h"
#include <string.h>
#include <queue>

#define NOTIFICATION_TIME 4

class Notification : public Window
{
	queue<string> q;
	Label* text;
	double updateTime;

public:
	Notification(void);
	~Notification(void);

	void notify(string msg);
	void Draw(GL2DProgram * shader) override;
};