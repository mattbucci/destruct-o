#include "stdafx.h"
#include "RasterizedText.h"


//Rasterize text for drawing
RasterizedText::RasterizedText(string text, Font * font, vec4 color) : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	this->text = text;
	this->textureId = 0;
	this->font = font;
	this->color = color;

	if (text == "")
		return;
	rasterize();
}
//Build empty rasterized text
RasterizedText::RasterizedText() : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	text = "";
	this->textureId = 0;
	this->font = NULL;
}
//For copying
RasterizedText::RasterizedText(const RasterizedText & original) : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	text = original.text;
	font = original.font;
	color = original.color;
	this->textureId = 0;

	if (text == "")
		return;
	rasterize();
}
RasterizedText & RasterizedText::operator=(const RasterizedText & original) {
	if (text != "") {
		//Cleanup existing texture data
		if (textureId > 0)
			glDeleteTextures(1,&textureId);
		textureId = 0;
	}

	text = original.text;
	font = original.font;
	color = original.color;

	if (text != "")
		rasterize();

	return *this;
}

RasterizedText::~RasterizedText() {
	if (textureId > 0) {
		//Cleanup existing texture data
		glDeleteTextures(1,&textureId);
		textureId = 0;
	}
}

//Build a flat box the same size as the font
void RasterizedText::setupVerticesAtSize(int width, int height) {
	//Setup the vertices
	svat(0,vec2(0,0));
	svat(1,vec2(0,height));
	svat(2,vec2(width,0));
	svat(3,vec2(width,height));	
	sxat(0,vec2(0,0));
	sxat(1,vec2(0,1));
	sxat(2,vec2(1,0));
	sxat(3,vec2(1,1));
}


//Rasterize the given text
void RasterizedText::rasterize() {

	//cout << "Rasterized: " << text << "\n";
 
	//Rasterize using the font
	SDL_Surface* surface = font->DrawToSurface(color,text);

	//Convert the surface to a gltexture
	glActiveTexture(GL_TEXTURE0);
	//glEnable(GL_TEXTURE_2D);
	if (textureId != 0)
		glDeleteTextures(1,&textureId);
	glGenTextures(1, &textureId);
	/*//Generate a texture if you haven't already
	if (textureId == 0) {
		cout << "NewTexture: " << textureId << "\n";
		
	}
	else {
		cout << "NewTexture: " << textureId << "\n";
	}*/
		
	glBindTexture(GL_TEXTURE_2D, textureId);

		
	//Is actually stored in  GL_BGRA but that can't be loaded in opengl es apparently
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
	//Build the gl shape to fit the text
	size = font->GetSizeString(text);
	setupVerticesAtSize((int)size.x,(int)size.y);

	//Cleanup surface
	SDL_FreeSurface(surface);
}

void RasterizedText::SetText(string text) {
	this->text = text;
	if (text != "")
		rasterize();
}

//Get the original text
string RasterizedText::Text() {
	return text;
}

//Draw the rasterized text (must use 2d shader)
void RasterizedText::Draw(GL2DProgram * shader) {

	//Check that there's something to draw
	if (text == "")
		return;

	//Mark textures as enabled in the shader
	shader->EnableTexture(true);

	//Set white color so the color isn't blended
	shader->SetColor(vec4(1,1,1,1));

	glActiveTexture(GL_TEXTURE0);
	//glEnable(GL_TEXTURE_2D);
	//Bind texture
	glBindTexture(GL_TEXTURE_2D,textureId);
	
	//Rescale the box so the height is the requested font height
	float scale = (float)font->GetFontSize()/size.y;
	shader->Model.PushMatrix();
	shader->Model.Scale(scale,scale,scale);
	shader->Model.Apply();

	GL2DVertexGroup::Draw(shader);

	//Turn textures back off
	shader->EnableTexture(false);
	//reset the model matrix
	shader->Model.PopMatrix();
}

vec2 RasterizedText::Size() {
	if (font != NULL) {
		//Recalculate size with the correct scale factor
		float scale = (float)font->GetFontSize()/size.y;
		return size * scale;
	}
	//No text means no font
	return vec2();
}