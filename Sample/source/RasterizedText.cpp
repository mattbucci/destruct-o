#include "stdafx.h"
#include "RasterizedText.h"


//Rasterize text for drawing
RasterizedText::RasterizedText(string text, Font * font, vec4 color) : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	this->text = text;
	if (text == "")
		return;

	this->font = font;
	this->color = color;
	rasterize();
}
//Build empty rasterized text
RasterizedText::RasterizedText() : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	text = "";
}
//For copying
RasterizedText::RasterizedText(const RasterizedText & original) : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	text = original.text;
	if (text == "")
		return;

	font = original.font;
	color = original.color;
	rasterize();
}
RasterizedText & RasterizedText::operator=(const RasterizedText & original) {
	if (text != "") {
		//Cleanup existing texture data
		glDeleteTextures(1,&textureId);
	}
	text = original.text;
	if (text == "")
		return *this;


	font = original.font;
	color = original.color;
	rasterize();

	return *this;
}

RasterizedText::~RasterizedText() {
	if (text != "") {
		//Cleanup existing texture data
		glDeleteTextures(1,&textureId);
	}
}

//Build a flat box the same size as the font
void RasterizedText::setupVerticesAtSize(int width, int height) {
	//Setup the vertices
	vat(0) = vec2(0,0);
	vat(1) = vec2(0,height);
	vat(2) = vec2(width,0);
	vat(3) = vec2(width,height);	
	xat(0) = vec2(0,0);
	xat(1) = vec2(0,1);
	xat(2) = vec2(1,0);
	xat(3) = vec2(1,1);
}


//Rasterize the given text
void RasterizedText::rasterize() {

	//cout << "Rasterized: " << text << "\n";
 
	//Rasterize using the font
	SDL_Surface* surface = font->DrawToSurface(color,text);

	//Convert the surface to a gltexture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

		
 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	//Build the gl shape to fit the text
	size = font->GetSizeString(text);
	setupVerticesAtSize((int)size.x,(int)size.y);

	//Cleanup surface
	SDL_FreeSurface(surface);
}

//Get the original text
string RasterizedText::Text() {
	return text;
}

//Draw the rasterized text (must use 2d shader)
void RasterizedText::Draw(GL2DProgram * shader) {

	//Mark textures as enabled in the shader
	shader->EnableTexture(true);

	//Set white color so the color isn't blended
	shader->SetColor(vec4(1,1,1,1));

	glActiveTexture (GL_TEXTURE0);
	//Bind texture
	glBindTexture(GL_TEXTURE_2D,textureId);
	
	//Rescale the box so the height is the requested font height
	float scale = (float)font->GetFontSize()/size.y;
	shader->Model.PushMatrix();
	shader->Model.Scale(scale,scale,scale);
	shader->Model.Apply();

	GL2DVertexGroup::Draw();

	//Turn textures back off
	shader->EnableTexture(false);
	//reset the model matrix
	shader->Model.PopMatrix();
}

vec2 RasterizedText::Size() {
	return size;
}