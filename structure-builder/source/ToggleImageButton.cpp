#include "stdafx.h"
#include "ToggleImageButton.h"
#include "Frames.h"
#include "GameSystem.h"
#include "TextureCache.h"

void ToggleImageButton::init() {
	Subscribe<void(Button*)>(&EventClicked,[this](Button * b) {
		SetValue(!value);
		if (CBValueChanged)
			CBValueChanged(value);
	});
	//Start off
	value = false;
}

ToggleImageButton::ToggleImageButton() {
	init();
}
ToggleImageButton::ToggleImageButton(Rect position) : Button(position) {
	init();
}
ToggleImageButton::~ToggleImageButton() {

}

//function<void()> CBValueChanged;

void ToggleImageButton::SetTexture(string texture) {
	this->texture = texture;
}

bool ToggleImageButton::GetValue() {
	return value;
}
void ToggleImageButton::SetValue(bool value) {
	this->value = value;
	if (value)
		tintColor = vec4(1,1,1,1);
	else
		tintColor = vec4(.3,.3,.3,1);
}


void ToggleImageButton::drawBackground(GL2DProgram * shaders) {
	shaders->Model.Apply();
	//Draw the classic square
	vat(0) = vec2(0,0);
	vat(1) = vec2(0,calculatedPosition.Height);
	vat(2) = vec2(calculatedPosition.Width,0);
	vat(3) = vec2(calculatedPosition.Width,calculatedPosition.Height);	
	//Enable the texture
	CurrentSystem->Textures.GetTexture(texture)->Bind();
	shaders->SetColor(tintColor);
	shaders->EnableTexture(true);

	//Draw the square
	GL2DVertexGroup::Draw(shaders);
}