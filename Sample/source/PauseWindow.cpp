#include "stdafx.h"

#include "PauseWindow.h"
#include "BaseFrame.h"

typedef pair<string,int> intOption;

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
	AddChild(&optsRect);

	//Options Back Button
	optsClose.vPin = Control::MIN;
	optsClose.hPin = Control::CENTER;
	optsClose.position = Rect(0, 370, 300, 40);
	optsClose.SetText("Back");
	optsRect.AddControl(&optsClose);
	Subscribe<void(Button*)>(&optsClose.EventClicked, [this](Button * b) {
		this->hideOptsMenu();
	});

	//Options View Distance
	optsViewDistanceLabel.vPin = optsViewDistance.vPin = Control::MIN;
	optsViewDistanceLabel.hPin = optsViewDistance.hPin = Control::CENTER;
	optsViewDistanceLabel.position = Rect(0, 50 + 80 * 0, 300, 20);
	optsViewDistance.SetPosition(Rect(0, 80 + 80 * 0, 300, 40));
	optsViewDistanceLabel.SetText("View Distance");
	optsRect.AddControl(&optsViewDistanceLabel);
	optsRect.AddControl(&optsViewDistance);

	intOption viewDistOpts[] = {
		intOption("Short", 0),
		intOption("Normal", 1),
		intOption("Far", 2)
	};
	vector<intOption> viewDistance(viewDistOpts, viewDistOpts + sizeof(viewDistOpts) / sizeof(viewDistOpts[0]));
	optsViewDistance.SetElements(viewDistance);
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