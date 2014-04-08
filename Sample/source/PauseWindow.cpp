#include "stdafx.h"

#include "PauseWindow.h"
#include "BaseFrame.h"

PauseWindow::PauseWindow(void) {

}

PauseWindow::PauseWindow(BaseFrame* parent)
{
	this->parent = parent;

	//Set up Self
	position = Rect(0, 30, 800, 570);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);
	SetVisible(false);

	//Set up Main Menu
	menuRect.position = Rect(0, 0, 320, 420);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Main Menu Children
	save.vPin = load.vPin = options.vPin = saveandquit.vPin = Control::CENTER;
	save.hPin = load.hPin = options.hPin = saveandquit.hPin = Control::CENTER;
	save.position =			Rect(0, -40 + 50 * -2, 300, 40);
	load.position =			Rect(0, -40 + 50 * -1, 300, 40);
	options.position =		Rect(0, -40 + 50 * 0, 300, 40);
	saveandquit.position =	Rect(0, 180, 300, 40);
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
	menuRect.SetVisible(false);

	//Subscribe Main Menu Buttons to Actions
	Subscribe<void(Button*)>(&save.EventClicked, [this](Button * b) {
		if(this->parent->Save("default")) {
			cout << "Save Successful!" << endl;
		} else {
			cout << "Save Unsuccessful." << endl;
		}
	});
	Subscribe<void(Button*)>(&load.EventClicked, [this](Button * b) {
		if(this->parent->Load("default")) {
			cout << "Load Successful!" << endl;
		} else {
			cout << "Load Unsuccessful." << endl;
		}
	});
	Subscribe<void(Button*)>(&options.EventClicked, [this](Button * b) {
		this->showOptsMenu();
	});
	Subscribe<void(Button*)>(&saveandquit.EventClicked, [this](Button * b) {
		cout << "\'Save & Quit\' Button Clicked" << endl;
		this->parent->Save("default");
		exit(0);
	});

	//Set up Options Menu
	optsRect.position = Rect(0, 0, 320, 420);
	optsRect.vPin = optsRect.hPin = Control::CENTER;
	optsRect.SetTitle("Options");
	optsRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	optsRect.SetVisible(false);

	//Set up Options Menu Children
	optsViewdistanceUp.vPin = optsViewdistanceDown.vPin = optsClose.vPin = Control::CENTER;
	optsViewdistanceUp.hPin = optsViewdistanceDown.hPin = optsClose.hPin = Control::CENTER;
	optsViewdistanceUp.position =	Rect(130, -40 + 50 * -2, 40, 40);
	optsViewdistanceDown.position =	Rect(-130, -40 + 50 * -2, 40, 40);
	optsClose.position =			Rect(0, 180, 300, 40);
	optsViewdistanceUp.SetText("+");
	optsViewdistanceDown.SetText("-");
	optsClose.SetText("Back");

	//Add Controls
	AddChild(&optsRect);
	optsRect.AddControl(&optsViewdistanceUp);
	optsRect.AddControl(&optsViewdistanceDown);
	optsRect.AddControl(&optsClose);

	//Subscribe Options Menu Buttons to Actions
	Subscribe<void(Button*)>(&optsViewdistanceUp.EventClicked, [this](Button * b) {
		cout << "\'Increase Viewdistance\' Button Clicked" << endl;
	});
	Subscribe<void(Button*)>(&optsViewdistanceDown.EventClicked, [this](Button * b) {
		cout << "\'Decrease Viewdistance\' Button Clicked" << endl;
	});
	Subscribe<void(Button*)>(&optsClose.EventClicked, [this](Button * b) {
		this->hideOptsMenu();
	});
}


PauseWindow::~PauseWindow(void)
{
}

bool PauseWindow::toggle() {
	if(menuRect.GetVisible()) {
		SetVisible(false);
		menuRect.SetVisible(false);
		return false;
	} else {
		SetVisible(true);
		menuRect.SetVisible(true);
		return true;
	}
}

void PauseWindow::showOptsMenu() {
	optsRect.SetVisible(true);
}

void PauseWindow::hideOptsMenu() {
	optsRect.SetVisible(false);
}