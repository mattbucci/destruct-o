#pragma once

#include "Control.h"
#include "Label.h"
#include "GameEvent.h"

class Button : public Control {
	vec4 normalColor;
	vec4 hoverColor;
	vec4 clickColor;
	Label innerLabel;
protected:
	virtual void OnMousePress(vec2 mousePos, int button, bool down);
	virtual void OnMouseMove(vec2 mousePos);
	virtual void OnMouseLeave();
	virtual void OnMouseEnter();
	virtual void OnButtonPress();

public:
	Button();
	Button(Rect position);
	Button(Rect position, string text);
	~Button();

	GameEvent<void(Button*)> EventClicked;

	void SetText(string text);
//Get
	string GetLabelText();
};