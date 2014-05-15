#include "stdafx.h"

#include "Notification.h"
#include "BaseFrame.h"
#include "Frames.h"

Notification::Notification(void) {
	w = 800;
	position = Rect(0, 0, (float)w, 20.0f);
	this->color = vec4(1.0f, 1.0f, 1.0f, 0.65f);
	hPin = Control::CENTER;
	SetVisible(true);
	text = new Label(0,0,"");
	text->hPin = Control::CENTER;
	text->SetText("");
	text->SetVisible(false);
	this->AddChild(text);
	updateTime = 0;
#ifndef __MOBILE__
	this->takeInput = false;
#endif
}

Notification::~Notification(void)
{
	delete text;
}

void Notification::Notify(string msg) {
	q.push(msg);
}

//If the user clicks on this window switch to the upgrade frame
void Notification::OnMousePress(vec2 mousePos, int button, bool down) {
#ifdef __MOBILE__
	//Switch to the upgrade menu
	if (down)
		Frames::SetSystem(Frames::FRAME_UPGRADEMENU);
#endif
}

void Notification::Draw(GL2DProgram * shader) {
	if(w != getParent()->position.Width) {
		w = (int)getParent()->position.Width;
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
	position = Rect(0, 0, (float)w, 20);
}