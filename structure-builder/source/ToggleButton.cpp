#include "stdafx.h"
#include "ToggleButton.h"


void ToggleButton::init() {
	Subscribe<void(Button*)>(&EventClicked,[this](Button * b) {
		SetValue(!value);
		if (CBValueChanged)
			CBValueChanged(value);
	});
	//Start off
	value = false;
}

ToggleButton::ToggleButton() {
	init();
}
ToggleButton::ToggleButton(Rect position) : Button(position) {
	init();
}
ToggleButton::~ToggleButton() {

}

//function<void()> CBValueChanged;

void ToggleButton::SetToggleText(string on, string off) {
	ontext = on;
	offtext = off;
	if (value)
		SetText(ontext);
	else
		SetText(offtext);
}

bool ToggleButton::GetValue() {
	return value;
}
void ToggleButton::SetValue(bool value) {
	this->value = value;
	if (value)
		SetText(ontext);
	else
		SetText(offtext);
}