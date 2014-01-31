#include "stdafx.h"
#include "Font.h"

#include <fstream>



Font::Font(string fontPath, int fontSize) {
	sdlfont = NULL;
	//Load font
	sdlfont = TTF_OpenFont(fontPath.c_str(),fontSize);
	if (!sdlfont) {
		cout << "Failed to load font " << fontPath << " [b]: " << TTF_GetError() << "\n";
		return;
	}

	TTF_SetFontHinting(sdlfont, TTF_HINTING_LIGHT);
	fontHeight = fontSize;
}
Font::~Font() {
	//Destroy font
	TTF_CloseFont(sdlfont);
}

int Font::GetFontSize() {
	return fontHeight;
}

vec2 Font::GetSizeString(string text) {
	/*float width = 0;
	float widthPart = 0;
	float height = fontSize;
	//Count the length of the string one character at a time
	for (int i = 0; i < text.length(); i++) {
		if (text[i] == '\n') {
			width = max(width,widthPart);
			widthPart = 0;
			height += fontSize;
		}
		else
			widthPart += getLetter(text[i])->GetWidth()-.3;
	}
	width = max(width,widthPart);
	return vec2((float)(width*fontSize+.15*fontSize),height);*/
	//For now we'll skip word-wrap support
	int width, height;
	TTF_SizeUTF8(sdlfont,text.c_str(),&width,&height);
	return vec2(width,height);
}
SDL_Surface * Font::DrawToSurface(vec4 color, string text) {
	/*shader->SetTexture(fontSheet);
	shader->SetColor(color);
	shader->Model.PushMatrix();
	shader->Model.Scale(fontSize,fontSize,1);

	Letter * lx;
	float currentWidth = 0;
	//Count the length of the string one character at a time
	for (int i = 0; i < text.length(); i++) {
		shader->Model.Apply();
		if (text[i] == '\n') {
			shader->Model.Translate(-currentWidth,1,0);
			currentWidth = 0;
		}
		else {
			lx = getLetter(text[i]);
			//Draw the letter in question
			lx->Draw();

			//Move over for the next letter
			shader->Model.Translate(lx->GetWidth()-.3,0,0);
			currentWidth += lx->GetWidth()-.3;
		}
	}
	shader->Model.PopMatrix();*/
	//Again skipping wordwrap for now
	SDL_Color scolor;
	scolor.a = (int)(color.a*255.0f);
	scolor.r = (int)(color.r*255.0f);
	scolor.g = (int)(color.g*255.0f);
	scolor.b = (int)(color.b*255.0f);

	return TTF_RenderText_Blended(sdlfont,text.c_str(),scolor);
}