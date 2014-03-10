
#pragma once

#include "Button.h"
#include "GameEventSubscriber.h"

class ToggleButton : public Button, public GameEventSubscriber {
	void init();
protected:
	bool value;
	string ontext;
	string offtext;
public:
	ToggleButton();
	ToggleButton(Rect position);
	~ToggleButton();

	function<void(bool value)> CBValueChanged;

	void SetToggleText(string on, string off);

	bool GetValue();
	void SetValue(bool value);
};