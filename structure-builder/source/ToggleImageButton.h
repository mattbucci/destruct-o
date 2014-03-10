
#pragma once

#include "Button.h"

class ToggleImageButton : public Button, public GameEventSubscriber {
	void init();
protected:
	bool value;
	string texture;
	vec4 tintColor;

	virtual void drawBackground(GL2DProgram * shaders);
public:
	ToggleImageButton();
	ToggleImageButton(Rect position);
	~ToggleImageButton();

	function<void(bool value)> CBValueChanged;

	void SetTexture(string texture);

	bool GetValue();
	void SetValue(bool value);
};