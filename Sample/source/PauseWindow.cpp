#include "stdafx.h"

#include "PauseWindow.h"
#include "BaseFrame.h"

PauseWindow::PauseWindow(void) {

}

PauseWindow::PauseWindow(BaseFrame* parent)
{
	w = 0, h = 0;
	this->p = parent;

	//Set up Self
	position = Rect(0, 20, 800, 580);
	color = vec4(0.33f, 0.33f, 0.33f, 0.75f);
	SetVisible(false);

	//Set up Main Menu
	menuRect.position = Rect(0, 0, 320, 420);
	menuRect.vPin = menuRect.hPin = Control::CENTER;
	menuRect.SetTitle("Menu");
	menuRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Set up Main Menu Children
	menuSave.vPin = menuLoad.vPin = menuOptions.vPin = menuSaveAndQuit.vPin = menuExit.vPin = Control::MIN;
	menuSave.hPin = menuLoad.hPin = menuOptions.hPin = menuSaveAndQuit.hPin = menuExit.hPin = Control::CENTER;
	menuSave.position =			Rect(0, 50 + 50 * 0, 300, 40);
	menuLoad.position =			Rect(0, 50 + 50 * 1, 300, 40);
	menuOptions.position =		Rect(0, 50 + 50 * 2, 300, 40);
	menuSaveAndQuit.position =	Rect(0, 50 + 50 * 3, 300, 40);
	menuExit.position =			Rect(0, 370, 300, 40);
	menuSave.SetText("Save");
	menuLoad.SetText("Load");
	menuOptions.SetText("Options");
	menuSaveAndQuit.SetText("Save & Quit");
	menuExit.SetText("Back to Game");

	//Add Controls
	AddChild(&menuRect);
	menuRect.AddControl(&menuSave);
	menuRect.AddControl(&menuLoad);
	menuRect.AddControl(&menuOptions);
	menuRect.AddControl(&menuSaveAndQuit);
	menuRect.AddControl(&menuExit);
	menuRect.SetVisible(false);

	//Subscribe Main Menu Buttons to Actions
	Subscribe<void(Button*)>(&menuSave.EventClicked, [this](Button * b) {
		if(this->p->Save("testsavefile.json.compressed")) {
			cout << "Save Successful!" << endl;
		} else {
			cout << "Save Unsuccessful." << endl;
		}
	});
	Subscribe<void(Button*)>(&menuLoad.EventClicked, [this](Button * b) {
		if(this->p->Load("testsavefile.json.compressed")) {
			cout << "Load Successful!" << endl;
		} else {
			cout << "Load Unsuccessful." << endl;
		}
	});
	Subscribe<void(Button*)>(&menuOptions.EventClicked, [this](Button * b) {
		this->showOptsMenu();
	});
	Subscribe<void(Button*)>(&menuSaveAndQuit.EventClicked, [this](Button * b) {
		cout << "\'Save & Quit\' Button Clicked" << endl;
		this->p->Save("testsavefile.json.compressed");
		exit(0);
	});
	Subscribe<void(Button*)>(&menuExit.EventClicked, [this](Button * b) {
		this->toggle();
	});

	//Set up Options Menu
	optsRect.position = Rect(0, 0, 320, 420);
	optsRect.vPin = optsRect.hPin = Control::CENTER;
	optsRect.SetTitle("Options");
	optsRect.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	optsRect.SetVisible(false);

	//Set up Options Menu Children
	optsViewdistanceUp.vPin = optsViewdistanceDown.vPin = optsClose.vPin = Control::MIN;
	optsViewdistanceUp.hPin = optsViewdistanceDown.hPin = optsClose.hPin = Control::CENTER;
	optsViewdistanceUp.position =	Rect(130, 50 + 50 * 0, 40, 40);
	optsViewdistanceDown.position =	Rect(-130, 50 + 50 * 0, 40, 40);
	optsClose.position =			Rect(0, 370, 300, 40);
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

void PauseWindow::Draw(GL2DProgram * shaders) {
	if(w != (int)getParent()->position.Width || h != (int)getParent()->position.Height) {
		w = getParent()->position.Width;
		h = getParent()->position.Height;
		resizeElement();
	}
	Window::Draw(shaders);
}

void PauseWindow::showOptsMenu() {
	optsRect.SetVisible(true);
}

void PauseWindow::hideOptsMenu() {
	optsRect.SetVisible(false);
}

void PauseWindow::resizeElement() {
	position = Rect(0, 20, w, h-20);
}