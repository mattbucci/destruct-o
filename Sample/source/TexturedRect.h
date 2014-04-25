
#pragma once
#include "stdafx.h"
#include "Rect.h"
#include "GL2DVertexGroup.h"

class GL2DProgram;

class TexturedRect : GL2DVertexGroup {
	string texturePath;
	vec4 tint;
    Rect rI;
public:
	TexturedRect(Rect r, string texturePath, vec4 initialColor = vec4(1,1,1,1));
	~TexturedRect();
	//Change the rect
	void SetRect(Rect r);
	//Change the rect + texture bounds
	void SetRectandTexRect(Rect r, Rect t);
	//Change the color tint
	void SetColor(vec4 color);
    
    // Change the texture
    void SetTexture(string texturePath);
    
    // Get the rectangle
    const Rect& GetRect();
    
	//Draw the rect now
	void Draw(GL2DProgram * program);
};