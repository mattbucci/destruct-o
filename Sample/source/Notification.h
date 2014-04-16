#pragma once

#include "Window.h"
#include "Label.h"
#include <string.h>
#include <queue>

#define NOTIFICATION_TIME 4

class Notification : public Window
{
	int w;

	queue<string> q;
	Label* text;
	double updateTime;

public:
	Notification(void);
	~Notification(void);

	void Notify(string msg);
	void Draw(GL2DProgram * shader) override;

private:
	void resizeElement();
};