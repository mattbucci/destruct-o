#include "stdafx.h"
#include "NewStructure.h"
#include "Frames.h"
#include "GameSystem.h"
#include "ErrorWindow.h"
#include "Structure.h"

NewStructure::NewStructure(function<void(Structure*)> onCloseWindow) {
	/*
	ComboBox sizeBox;
	ComboBox heightBox;
	Button ok;
	Button cancel;
	Label sizeLabel;
	Label heightLabel;
	*/

	//Set Window Position
	position = Rect(0,0,300,240);
	hPin = Control::CENTER;
	vPin = Control::CENTER;
	SetTitle("New Structure");

	//add in boxes
	sizeBox.position = Rect(40,70,100,30);
	sizeBox.AddComboEntry("10");
	sizeBox.AddComboEntry("20");
	sizeBox.AddComboEntry("30");
	sizeBox.AddComboEntry("40");
	sizeBox.AddComboEntry("50");
	heightBox.position = Rect(170,70,100,30);
	heightBox.AddComboEntry("5");
	heightBox.AddComboEntry("10");
	heightBox.AddComboEntry("20");
	heightBox.AddComboEntry("30");
	heightBox.AddComboEntry("40");

	//labels for the boxes
	sizeLabel.position = Rect(40,50,0,0);
	sizeLabel.SetText("Base Size");
	heightLabel.position = Rect(170,50,0,0);
	heightLabel.SetText("Height");

	//Buttons
	ok.vPin = Control::MAX;
	cancel.vPin = Control::MAX;
	ok.hPin = Control::MAX;
	cancel.position = Rect(10,-10,70,30);
	ok.position = Rect(-10,-10,70,30);

	cancel.SetText("Cancel");
	ok.SetText("Ok");

	//Handle button events
	Subscribe<void(Button*)>(&ok.EventClicked,[this,onCloseWindow](Button * b) {
		//They pressed ok, check that everything is in order
		int size = atoi(sizeBox.GetSelected().c_str());
		int height = atoi(heightBox.GetSelected().c_str());
		if ((size <= 0) || (height <= 0)) {
			ErrorWindow::ShowError("Error!","You must select size and height.");
			return;
		}
		//Must be valid
		//create the structure
		Structure * s = new Structure();
		s->Extents = vec3(size,size,height);
		onCloseWindow(s);
		//Close this window
		CurrentSystem->Controls.RequestControlDestroyed(this);
	});
	Subscribe<void(Button*)>(&cancel.EventClicked,[this,onCloseWindow](Button * b) {
		//Close this window
		CurrentSystem->Controls.RequestControlDestroyed(this);
	});


	//Add all controls
	AddChild(&sizeBox);
	AddChild(&heightBox);
	AddChild(&sizeLabel);
	AddChild(&heightLabel);
	AddChild(&ok);
	AddChild(&cancel);

}
NewStructure::~NewStructure() {

}

void NewStructure::ShowNewStructure(function<void(Structure*)> onCloseWindow) {
	CurrentSystem->Controls.AddWindow(new NewStructure(onCloseWindow));
}