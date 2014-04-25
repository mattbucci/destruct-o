#include "stdafx.h"
#include "TexturedRect.h"
#include "GL2DProgram.h"
#include "Frames.h"
#include "GameSystem.h"


TexturedRect::TexturedRect(Rect r, string texturePath, vec4 initialColor) : GL2DVertexGroup(GL_TRIANGLE_STRIP,4) {
	SetRect(r);

	tint = initialColor;
	this->texturePath = texturePath;
}
TexturedRect::~TexturedRect() {

}
//Change the rect
void TexturedRect::SetRect(Rect r) {
    rI = r;
	svat(0,vec2(r.X,r.Y));
	sxat(0,vec2(0,0));
	svat(1,vec2(r.X,r.Y+r.Height));
	sxat(1,vec2(0,1));
	svat(2,vec2(r.X+r.Width,r.Y));
	sxat(2,vec2(1,0));
	svat(3,vec2(r.X+r.Width,r.Y+r.Height));
	sxat(3,vec2(1,1));
}
//Change the rect + texture bounds
void TexturedRect::SetRectandTexRect(Rect r, Rect t) {
    rI = r;
	svat(0,vec2(r.X,r.Y));
	sxat(0,vec2(t.X,t.Y));
	svat(1,vec2(r.X,r.Y+r.Height));
	sxat(1,vec2(t.X,t.Height));
	svat(2,vec2(r.X+r.Width,r.Y));
	sxat(2,vec2(t.Width,t.Y));
	svat(3,vec2(r.X+r.Width,r.Y+r.Height));
	sxat(3,vec2(t.Width,t.Height));
}
//Change the color tint
void TexturedRect::SetColor(vec4 color) {
	tint = color;
}

// Change the texture
void TexturedRect::SetTexture(string texturePath)
{
    this->texturePath = texturePath;
}

// Get the rectangle
const Rect& TexturedRect::GetRect()
{
    return rI;
}

//Draw the rect now
void TexturedRect::Draw(GL2DProgram * program) {
	//Enable the texture and tint
	program->EnableTexture(true);
	program->SetColor(tint);
	CurrentSystem->Textures.GetTexture(texturePath)->Bind();
	//Draw the rect
	GL2DVertexGroup::Draw(program);
}