#include "stdafx.h"
#include "PictureButton.h"
#include "PictureBox.h"

void PictureButton::init() {
	picturebox = new PictureBox();
	SetPicturePosition(Rect(.25,.5,.5,.25));

	AddChild(picturebox);
}

PictureButton::PictureButton() {
	init();
}
PictureButton::PictureButton(Rect position) : Button(position) {
	init();
}
PictureButton::PictureButton(Rect position, string text, string texture) : Button(position,text) {
	init();
}

void PictureButton::SetTexture(string texture) {
	picturebox->SetPicture(texture);
}
string PictureButton::GetTexture() {
	return picturebox->GetPicture();
}

//This sets the relative position of the picture in 
//% of the buttons size, so X,Y,width,height should all be 0-1
void PictureButton::SetPicturePosition(Rect picturePosition) {
	picturebox->position = Rect(picturePosition.X*position.Width,picturePosition.Y*position.Height,picturePosition.Width*position.Width,picturePosition.Height*position.Height);
}

//Move the height of the text in % (always centered horizontally)
void PictureButton::SetTextPosition(float height) {
	Button::innerLabel.vPin = Control::MIN;
	Button::innerLabel.position = Rect(0,height*position.Height,0,0);
}