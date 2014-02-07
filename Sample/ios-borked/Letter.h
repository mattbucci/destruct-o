#pragma once

#include "GL2DVertexGroup.h"

class Letter : public GL2DVertexGroup {
	float width,height;
public:
	//Represents a letter inside a texture at TX, TY, with width and height
	//coordinates are in texture coordinate format
	//the vertex width is the normalized width of the character. normalized height is always 1
	Letter(float tx, float ty, float twidth, float theight, float vertexWidth);

	float GetWidth();
	float GetHeight();
};