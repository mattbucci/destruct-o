#pragma once

#include "stdafx.h"
#include "GL2DProgram.h"
#include <SDL/SDL_ttf.h>


class Letter;
class GLTexture;


class Font {
	TTF_Font * sdlfont;
	int fontHeight;
public:
	Font(string fontPath, int fontSize);
	~Font();

	//Get the size (height) of the font
	int GetFontSize();

	//Get the size of the specified string
	vec2 GetSizeString(string text);

	//Draw the text with color to an SDL surface
	SDL_Surface * DrawToSurface(vec4 color, string text);
};