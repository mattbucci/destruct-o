#pragma once

#include "stdafx.h"
#include "Font.h"
#include "GL2DVertexGroup.h"

class RasterizedText : GL2DVertexGroup {
	//The texture from the converted surface
	GLuint textureId;
	string text;
	Font * font;
	vec4 color;
	//Build a flat box the same size as the font
	void setupVerticesAtSize(int width, int height);
	//Rasterize the given text
	void rasterize();
	//the size after rasterization
	vec2 size;
public:
	//Rasterize text for drawing
	//note, Font pointer should stay valid for the
	//entire life of the rasterized text!!
	RasterizedText(string text, Font * font, vec4 color = vec4(0,0,0,1));
	//Build empty rasterized text
	RasterizedText();
	//For copying
	RasterizedText(const RasterizedText & original);
	RasterizedText & operator=(const RasterizedText & original);

	//Cleanup the gl texture
	~RasterizedText();

	//Change the text (automatically re-rasterizes)
	void SetText(string text);

	//Get the original text
	string Text();

	//Get the size of the rasterized text
	vec2 Size();

	//Draw the rasterized text (must use 2d shader)
	void Draw(GL2DProgram * shader);
};