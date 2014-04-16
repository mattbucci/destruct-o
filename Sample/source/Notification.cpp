#include "stdafx.h"

#include "Notification.h"
#include "BaseFrame.h"

Notification::Notification(void) {
	w = 800;
	position = Rect(0, 0, 800, 30);
	this->color = vec4(1.0f, 1.0f, 1.0f, 0.65f);
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

void Notification::Notify(string msg) {
	q.push(msg);
}

void Notification::Draw(GL2DProgram * shader) {
	if(w != getParent()->position.Width) {
		w = getParent()->position.Width;
		resizeElement();
	}
	if(Game()->Now() > updateTime) {
		if(q.size() > 0) {
			updateTime = Game()->Now() + NOTIFICATION_TIME;
			text->SetText(q.front());
			text->SetVisible(true);
			q.pop();
		} else {
			text->SetVisible(false);
		}
	}
	Window::Draw(shader);
}

void Notification::resizeElement() {
	position = Rect(0, 0, w, 30);
}