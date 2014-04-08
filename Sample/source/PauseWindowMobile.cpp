#include "stdafx.h"

#include "PauseWindowMobile.h"
#include "BaseFrame.h"

PauseWindowMobile::PauseWindowMobile(void) {

}

PauseWindowMobile::PauseWindowMobile(BaseFrame* parent) : PauseWindow(parent)
{
	//Set up Self
	position = Rect(0, 30, 800, 570);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);

	//Set up Main Menu
	menuRect.position = Rect(0, 0, 320, 360);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Main Menu Children
	save.vPin = load.vPin = options.vPin = saveandquit.vPin = Control::CENTER;
	save.hPin = load.hPin = options.hPin = saveandquit.hPin = Control::CENTER;
	save.position =			Rect(-77.5, -57.5 + 155 * 0, 145, 145);
	load.position =			Rect(77.5, -57.5 + 155 * 0, 145, 145);
	options.position =		Rect(-77.5, -57.5 + 155 * 1, 145, 145);
	saveandquit.position =	Rect(77.5, -57.5 + 155 * 1, 145, 145);
	save.SetText("Save");
	load.SetText("Load");
	options.SetText("Options");
	saveandquit.SetText("Save & Quit");

	//Set up Options Menu
	optsRect.position = Rect(0, 0, 320, 360);
	optsRect.vPin = optsRect.hPin = Control::CENTER;
	optsRect.SetTitle("Options");
	optsRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Options Menu Children
	optsViewdistanceUp.vPin = optsViewdistanceDown.vPin = optsClose.vPin = Control::CENTER;
	optsViewdistanceUp.hPin = optsViewdistanceDown.hPin = optsClose.hPin = Control::CENTER;
	optsViewdistanceUp.position =	Rect(130, -40 + 50 * -2, 40, 40);
	optsViewdistanceDown.position =	Rect(-130, -40 + 50 * -2, 40, 40);
	optsClose.position =			Rect(0, 180, 300, 40);
	optsViewdistanceUp.SetText("+");
	optsViewdistanceDown.SetText("-");
	optsClose.SetText("Back");
}


PauseWindowMobile::~PauseWindowMobile(void)
{
}