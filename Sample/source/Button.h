#pragma once

#include "Control.h"
#include "Label.h"
#include "GameEvent.h"

class Button : public Control {
protected:
	vec4 normalColor;
	vec4 hoverColor;
	vec4 clickColor;
	Label innerLabel;

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
	void SetTextColor(vec4 color);
//Get
	string GetLabelText();
};