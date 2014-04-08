#include "stdafx.h"
#include "Notification.h"
#include "VoxEngine.h"

Notification::Notification(void) {
	position = Rect(0, 0, 800, 30);
	this->color = vec4(this->color.r, this->color.g, this->color.b, 0.85f);
	hPin = Control::CENTER;
	SetVisible(true);
	text = new Label(0,5,"");
	text->hPin = Control::CENTER;
	text->SetText("");
	text->SetVisible(false);
	this->takeInput = false;
	this->AddChild(text);
	updateTime = 0;
}


Notification::~Notification(void)
{
	delete text;
}

void Notification::notify(string msg) {
	q.push(msg);
}

void Notification::Draw(GL2DProgram * shader) {
	if(VoxEngine::GetGameSimTime() > updateTime) {
		if(q.size() > 0) {
			updateTime = VoxEngine::GetGameSimTime() + NOTIFICATION_TIME;
			text->SetText(q.front());
			text->SetVisible(true);
			q.pop();
		} else {
			text->SetVisible(false);
		}
	}
	Window::Draw(shader);
}