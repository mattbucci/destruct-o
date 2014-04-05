#include "stdafx.h"
#include "Notification.h"

Notification::Notification(void) {
	position = Rect(0, 0, 200, 30);
	hPin = Control::CENTER;
	SetVisible(false);
	text = new Label(0,5,"");
	text->hPin = Control::CENTER;
	text->SetText("");
	text->SetVisible(false);
	this->AddChild(text);
}


Notification::~Notification(void)
{
		delete text;
}

void Notification::notify(string msg) {
	text->SetText(msg);
	SetVisible(true);
	text->SetVisible(true);
}

void Notification::Draw(GL2DProgram * shader) {
	Window::Draw(shader);
}