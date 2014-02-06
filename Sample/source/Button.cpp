#include "stdafx.h"
#include "Button.h"

void Button::OnMousePress(vec2 mousePos, int button, bool down) {
	if (down) {
		color = clickColor;
		useNinePatch(VisualInterface.NPButtonDown);
	}
	else {
		OnButtonPress();
		color = hoverColor;
		useNinePatch(VisualInterface.NPButtonHover);
	}
		
}
void Button::OnMouseMove(vec2 mousePos) {

}
void Button::OnButtonPress() {
	EventClicked.Fire([](function<void()> eventSubscriber){
		eventSubscriber();
	});
}
void Button::OnMouseLeave() {
	useNinePatch(VisualInterface.NPButton);
	color = normalColor;
}
void Button::OnMouseEnter() {
	useNinePatch(VisualInterface.NPButtonHover);
	color = hoverColor;
}


Button::Button() : innerLabel(0,0,"") {
	useNinePatch(VisualInterface.NPButton);
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = vec4(.2,.2,.2,1);
	hoverColor = vec4(.4,.4,.4,1);
	clickColor = vec4(.6,.6,.6,1);

	AddChild(&innerLabel);

	color = normalColor;
}

Button::Button(Rect position) : innerLabel(0,0,"") {
	useNinePatch(VisualInterface.NPButton);
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = vec4(.2,.2,.2,1);
	hoverColor = vec4(.4,.4,.4,1);
	clickColor = vec4(.6,.6,.6,1);

	AddChild(&innerLabel);

	color = normalColor;
	this->position = position;
}
Button::Button(Rect position, string text) : innerLabel(0,0,"") {
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = vec4(.2,.2,.2,1);
	hoverColor = vec4(.4,.4,.4,1);
	clickColor = vec4(.6,.6,.6,1);

	AddChild(&innerLabel);

	color = normalColor;
	this->position = position;
	SetText(text);
}

Button::~Button() {

}

void Button::SetText(string text) {
	innerLabel.SetText(text);
}

string Button::GetLabelText() {
	return innerLabel.GetText();
}