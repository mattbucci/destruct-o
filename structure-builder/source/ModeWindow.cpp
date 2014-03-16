#include "stdafx.h"
#include "ModeWindow.h"
#include "Editor.h"

static string modeNames[4] = {"Level by Level", "Build Up", "Sticky", "Delete"};

//Update which mode is shown to be active
void ModeWindow::Update() {
	int selectedMode = (int)structureEditor->GetMode();
	for (int i = 0; i < modes.size(); i++) {
		if (i == selectedMode)
			modes[i].SetText(string("->") + modeNames[i] + "<-");
		else
			modes[i].SetText(modeNames[i]);
	}
}

//4 modes currently
ModeWindow::ModeWindow(Editor * structureEditor) : modes(4) {
	//Set this windows position
	hPin = Control::CENTER;
	vPin = Control::MAX;
	position = Rect(0,-20,30+modes.size()*140,50);


	//Setup all the mode buttons
	for (int i = 0; i < modes.size(); i++) {
		//setup each button
		modes[i].vPin = Control::CENTER;
		modes[i].position = Rect(20+i*140,0,130,30);
		modes[i].SetText(modeNames[i]);
		
		//setup the on click
		Subscribe<void(Button*)>(&modes[i].EventClicked,[i,structureEditor](Button * b) {
			structureEditor->SetMode((Editor::EditorMode)i);
		});

		//Add the button to yourself
		AddChild(&modes[i]);
	}

	this->structureEditor = structureEditor;

}
ModeWindow::~ModeWindow() {

}