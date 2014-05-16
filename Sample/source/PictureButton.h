
#pragma once

#include "Button.h"

class PictureBox;

class PictureButton : public Button {
	string texture;
	Rect textureRelativePos;

	PictureBox * picturebox;

	void init();
public:
	PictureButton();
	PictureButton(Rect position);
	PictureButton(Rect position, string text, string texture);

	void SetTexture(string texture);
	string GetTexture();

	//This sets the relative position of the picture in 
	//% of the buttons size, so X,Y,width,height should all be 0-1
	void SetPicturePosition(Rect position);

	//Move the height of the text in % (always centered horizontally)
	void SetTextPosition(float height);
};