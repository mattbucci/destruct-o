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
protected:
	//If the user clicks on this window switch to the upgrade frame
	virtual void OnMousePress(vec2 mousePos, int button, bool down) override;
public:
	Notification(void);
	~Notification(void);

	void Notify(string msg);
	void Draw(GL2DProgram * shader) override;

private:
	void resizeElement();
};