
#pragma once

#include "stdafx.h"

#include "GL2DVertexGroup.h"
#include "RasterizedText.h"

class Font;
class GLTexture;

class LoadingScreen : public GL2DVertexGroup {
	//Fonts
	static Font * titleText;
	static Font * infoText;
	static GLTexture * texture;

	RasterizedText bigText;
	RasterizedText littleText;
public:
	LoadingScreen();
	~LoadingScreen();

	//Draw the loading screen
	void Draw(int width, int height, GL2DProgram * shader);
};