#include "stdafx.h"

#include "PauseWindowMobile.h"
#include "BaseFrame.h"

PauseWindowMobile::PauseWindowMobile(void) {

}

PauseWindowMobile::PauseWindowMobile(BaseFrame* parent) : PauseWindow(parent)
{
	//On Screen Buttons Container
	onScreenButtonsRect.position = Rect(0, 30, 60, 60);
	onScreenButtonsRect.vPin = Control::MIN;
	onScreenButtonsRect.hPin = Control::MAX;
	onScreenButtonsRect.color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	onScreenButtonsRect.SetVisible(true);
	parent->Controls.AddWindow(&onScreenButtonsRect);

	//Set up On Screen Buttons
	menuButton.position = Rect(5, 5, 50, 50);
	menuButton.vPin = Control::MIN;
	menuButton.hPin = Control::MIN;
	menuButton.SetText("Menu");

	Subscribe<void(Button*)>(&menuButton.EventClicked, [this](Button * b) {
		this->toggle();
	});

	//Diable Menu Button for Demo
	//onScreenButtonsRect.AddControl(&menuButton);

	//Set up Self
	position = Rect(0, 30, 800, 570);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);

	//Set up Menu
	menuRect.position = Rect(0, 0, 320, 420);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Menu Exit Button
	menuExit.vPin = Control::MIN;
	menuExit.hPin = Control::CENTER;
	menuExit.position = Rect(0, 50 + 155 * 2, 300, 50);
	menuExit.SetText("Back to Game");

	//Set up Main Menu Children
	menuSave.vPin = menuLoad.vPin = menuOptions.vPin = menuSaveAndQuit.vPin = menuExit.vPin = Control::MIN;
	menuSave.hPin = menuLoad.hPin = menuOptions.hPin = menuSaveAndQuit.hPin = menuExit.hPin = Control::CENTER;
	menuSave.position =			Rect(-77.5, 50 + 155 * 0, 145, 145);
	menuLoad.position =			Rect(77.5, 50 + 155 * 0, 145, 145);
	menuOptions.position =		Rect(-77.5, 50 + 155 * 1, 145, 145);
	menuSaveAndQuit.position =	Rect(77.5, 50 + 155 * 1, 145, 145);
	menuExit.position =			Rect(0, 50 + 155 * 2, 300, 50);
	menuSave.SetText("Save");
	menuLoad.SetText("Load");
	menuOptions.SetText("Options");
	menuSaveAndQuit.SetText("Save & Quit");
	menuExit.SetText("Back to Game");

	//Set up Options Menu
	optsRect.position = Rect(0, 0, 320, 420);
	optsRect.vPin = optsRect.hPin = Control::CENTER;
	optsRect.SetTitle("Options");
	optsRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Options Menu Children
	/*optsViewdistanceUp.vPin = optsViewdistanceDown.vPin = optsClose.vPin = Control::MIN;
	optsViewdistanceUp.hPin = optsViewdistanceDown.hPin = optsClose.hPin = Control::CENTER;
	optsViewdistanceUp.position =	Rect(77.5, 50 + 77.5 * 1, 145, 67.5);
	optsViewdistanceDown.position =	Rect(-77.5, 50 + 77.5 * 1, 145, 67.5);*/
	optsClose.position =			Rect(0, 50 + 77.5 * 4, 300, 50);
	//optsViewdistanceUp.SetText("+");
	//optsViewdistanceDown.SetText("-");
	optsClose.SetText("Back");
}


PauseWindowMobile::~PauseWindowMobile(void)
{
}