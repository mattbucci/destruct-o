#include "PauseWindow.h"


PauseWindow::PauseWindow(void)
{
	//Set up Self
	position = Rect(0, 0, 800, 600);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);

	//Set up Children
	menuRect.position = Rect(0, 0, 320, 420);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Children
	save.vPin = load.vPin = options.vPin = saveandquit.vPin = Control::CENTER;
	save.hPin = load.hPin = options.hPin = saveandquit.hPin = Control::CENTER;
	save.position =			Rect(0, -40 + 50 * -2, 300, 40);
	load.position =			Rect(0, -40 + 50 * -1, 300, 40);
	options.position =		Rect(0, -40 + 50 * -0, 300, 40);
	saveandquit.position =	Rect(0, -40 + 50 * 1, 300, 40);
	save.color
	save.normalColor =				vec4(0.33f, 0.33f, 0.33f, 1.0f);
	load.normalColor =				vec4(0.33f, 0.33f, 0.33f, 1.0f);
	options.normalCcolor =			vec4(0.33f, 0.33f, 0.33f, 1.0f);
	saveandquit.normalCcolor =		vec4(0.33f, 0.33f, 0.33f, 1.0f);
	save.SetText("Save");
	load.SetText("Load");
	options.SetText("Options");
	saveandquit.SetText("Save & Quit");

	//Add Controls
	AddChild(&menuRect);
	menuRect.AddControl(&save);
	menuRect.AddControl(&load);
	menuRect.AddControl(&options);
	menuRect.AddControl(&saveandquit);
}


PauseWindow::~PauseWindow(void)
{
}
