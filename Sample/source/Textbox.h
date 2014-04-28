#pragma once

#include "Control.h"

#include "GL2DProgram.h"
#include "Font.h"
#include "Shape.h"
#include "RasterizedText.h"

#include "GameEvent.h"

class Textbox : public Control {
    string::size_type cursorPos;

	Font textboxFont;

	vec4 textColor;
	vec4 borderColor;
	vec4 backColor;
	vec4 selectionColor;

    string::size_type maxTextLength;

	//This shape represents the inner color with this control's background
	//is the border color
	Shape innerShape;
	Shape selectionShape;
	//The text held by the textbox
	RasterizedText text;
protected:

	void Draw(GL2DProgram * shaders) override;
	void OnMousePress(vec2 mousePos, int button, bool down);
	void OnMouseMove(vec2 mousePos);
	void OnMouseLeave();
	void OnMouseEnter();

	void OnKeyPress(int key) override;

	virtual void OnTextChange(string newText);

public:
	Textbox();
	~Textbox();

	//Happens when text in the textbox is changed
	//string is the new contents of the textbox
	GameEvent<void(Textbox*,string)> EventTextChanged;

	void SetText(string text);
	void SetMaxLength(string::size_type length);
	//Get
	string GetText();
};