#pragma once

#include "stdafx.h"
#include "GLModel.h"
#include "Rect.h"

class GLClipping {
	vector<Rect> clippingRects;
	GLuint clippingUniform;
public:
	GLClipping(GLuint clippingUniform);

	void PushClippingRect(const GLModel & modelMatrix, Rect region);
	void PopClippingRect();
};