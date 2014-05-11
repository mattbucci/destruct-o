#include "stdafx.h"
#include "PictureBox.h"
#include "GL2DProgram.h"
#include "Frames.h"
#include "GameSystem.h"

void PictureBox::drawBackground(GL2DProgram * shaders) {
	//Draw the classic square
	svat(0,vec2(0,0));
	svat(1,vec2(0,calculatedPosition.Height));
	svat(2,vec2(calculatedPosition.Width,0));
	svat(3,vec2(calculatedPosition.Width,calculatedPosition.Height));	
	//Enable pure-color no texture
	shaders->SetColor(color);
	shaders->EnableTexture(true);
	//Bind the texture
	CurrentSystem->Textures.GetTexture<GLTexture>(texture)->Bind();
}

PictureBox::PictureBox() {

}
PictureBox::PictureBox(Rect position) : Control(position){
	
}
PictureBox::PictureBox(Rect position, string pictureTexture) : Control(position) {
	texture = pictureTexture;
}


void PictureBox::SetPicture(string texture) {
	this->texture = texture;
}
string PictureBox::GetPicture() {
	return texture;
}