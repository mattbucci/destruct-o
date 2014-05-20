#include "stdafx.h"
#include "Font.h"

#include <fstream>



Font::Font(string fontPath, int fontSize) {
	sdlfont = NULL;
	fontHeight = fontSize;
	//Load font
	sdlfont = TTF_OpenFont(fontPath.c_str(),fontSize);
	if (!sdlfont) {
		cout << "Failed to load font " << fontPath << " [b]: " << TTF_GetError() << "\n";
		return;
	}

	TTF_SetFontHinting(sdlfont, TTF_HINTING_LIGHT);
}
Font::~Font() {
	//Destroy font
	TTF_CloseFont(sdlfont);
}

int Font::GetFontSize() {
	return fontHeight;
}

vec2 Font::GetSizeString(string text) {
	//For now we'll skip word-wrap support
	int width, height;
	TTF_SizeUTF8(sdlfont,text.c_str(),&width,&height);
	return vec2(width,height);
}
SDL_Surface * Font::DrawToSurface(vec4 color, string text) {
	//Again skipping wordwrap for now
	SDL_Color scolor;
	scolor.a = (int)(color.a*255.0f);
	scolor.r = (int)(color.r*255.0f);
	scolor.g = (int)(color.g*255.0f);
	scolor.b = (int)(color.b*255.0f);

	return TTF_RenderText_Blended(sdlfont,text.c_str(),scolor);
}