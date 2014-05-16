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
	EventClicked.Fire([this](function<void(Button*)> eventSubscriber){
		eventSubscriber(this);
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


//On draw properly size the label
void Button::Draw(GL2DProgram * shaders) {
	innerLabel.SetMaxWidth(position.Width);
	Control::Draw(shaders);
}

Button::Button() : innerLabel(0,0,"") {
	useNinePatch(VisualInterface.NPButton);
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = VisualInterface.ColorControlBackground;
	hoverColor = VisualInterface.ColorControlHighlight;
	clickColor = VisualInterface.ColorControlSelection;

	AddChild(&innerLabel);

	color = normalColor;
}

Button::Button(Rect position) : innerLabel(0,0,"") {
	useNinePatch(VisualInterface.NPButton);
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = VisualInterface.ColorControlBackground;
	hoverColor = VisualInterface.ColorControlHighlight;
	clickColor = VisualInterface.ColorControlSelection;

	AddChild(&innerLabel);

	color = normalColor;
	this->position = position;
}
Button::Button(Rect position, string text) : innerLabel(0,0,"") {
	//center the label
	innerLabel.hPin = Control::CENTER;
	innerLabel.vPin = Control::CENTER;

	normalColor = VisualInterface.ColorControlBackground;
	hoverColor = VisualInterface.ColorControlHighlight;
	clickColor = VisualInterface.ColorControlSelection;

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

void Button::SetTextColor(vec4 color) {
	innerLabel.SetTextColor(color);
}

string Button::GetLabelText() {
	return innerLabel.GetText();
}