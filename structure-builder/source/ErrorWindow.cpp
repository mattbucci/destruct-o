#include "stdafx.h"
#include "ErrorWindow.h"

#include "GameSystem.h"
#include "Frames.h"

ErrorWindow::ErrorWindow(string title, string error) : Window(Rect(0,0,300,160),title), okButton(Rect(0,-10,60,40),"OK"),errorMessage(0,25,error) {
	//Center the buttons and label
	okButton.hPin = Control::CENTER;
	okButton.vPin = Control::MAX;
	errorMessage.hPin = Control::CENTER;
	errorMessage.SetMaxWidth(260);
	errorMessage.SetText(error); //Reset text so wordwrap functions
	this->position.Height = errorMessage.position.Height+80;
	Subscribe<void(Button*)>(&okButton.EventClicked,[this](Button * button) {
		//Queue yoruself for destruction
		CurrentSystem->Controls.RequestControlDestroyed(this);
		EventOkPressed.Fire([this](function<void(ErrorWindow*)> subscriber) {
			subscriber(this);
		});
	});
	//Add your inner controls
	AddChild(&okButton);
	AddChild(&errorMessage);
	//Center yourself
	hPin = Control::CENTER;
	vPin = Control::CENTER;
}
ErrorWindow::~ErrorWindow() {

}