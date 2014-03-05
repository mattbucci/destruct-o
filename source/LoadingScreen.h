
#pragma once

#include "stdafx.h"

#include "GL2DVertexGroup.h"

class LoadingScreen : public GL2DVertexGroup {
	GLuint textureId;
public:
	LoadingScreen();
	~LoadingScreen();

	//Draw the loading screen
	void Draw(int width, int height, GL2DProgram * shader);
};