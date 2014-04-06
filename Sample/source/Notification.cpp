#include "stdafx.h"
#include "Notification.h"

Notification::Notification(void) {
	position = Rect(0, 0, 800, 30);
	this->color = vec4(this->color.r, this->color.g, this->color.b, 0.7f);
	hPin = Control::CENTER;
	SetVisible(true);
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