#pragma once

#include "stdafx.h"
#include "GL2DProgram.h"

// If we are compiling on Mac OS X, use a different SDL2_ttf header, for we use the SDL2_ttf framework
#if (defined __MACOSX__)
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL2/SDL_ttf.h>
#endif

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
