#include "stdafx.h"
#include "PictureBox.h"
#include "GL2DProgram.h"
#include "Frames.h"
#include "GameSystem.h"

void PictureBox::drawBackground(GL2DProgram * shaders) {
	//Draw the classic square
	svat(0,vec2(0,0));
	sxat(0,vec2(0,0));
	svat(1,vec2(0,calculatedPosition.Height));
	sxat(1,vec2(0,1));
	svat(2,vec2(calculatedPosition.Width,0));
	sxat(2,vec2(1,0));
	svat(3,vec2(calculatedPosition.Width,calculatedPosition.Height));	
	sxat(3,vec2(1,1));
	//Enable color
	shaders->SetColor(color);
	shaders->EnableTexture(true);
	//Bind the texture
	CurrentSystem->Textures.GetTexture<GLTexture>(texture)->Bind();

	//Draw the picturebox
	shaders->Model.Apply();
	GL2DVertexGroup::Draw(shaders);
}

PictureBox::PictureBox() {
	//start pure white
	color = vec4(1,1,1,1);
}
PictureBox::PictureBox(Rect position) : Control(position){
	//start pure white
	color = vec4(1,1,1,1);	
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