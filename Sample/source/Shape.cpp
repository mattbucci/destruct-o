#include "stdafx.h"
#include "Shape.h"
#include "GL2DProgram.h"

//	int vertexCount;

Shape::Shape(int vertexCount) : Control(vertexCount) {
	this->vertexCount = vertexCount;
}
Shape::~Shape() {

}

//Calculate security based off of vertex information
void Shape::recalculatePosition() {
	Control::recalculatePosition();
}

void Shape::SetToSquare(void) {
	_ASSERTE(vertexCount == 4);
	vat(0) = vec2(0,0);
	vat(1) = vec2(1,0);
	vat(2) = vec2(0,1);
	vat(3) = vec2(1,1);
}

void Shape::SetToUpTriangle(void) {
	_ASSERTE(vertexCount == 3);
	vset(0) = vec2(10,0);
	vset(1) = vec2(20,20);
	vset(2) = vec2(0,20);
}
void Shape::SetToDownTriangle(void) {
	_ASSERTE(vertexCount == 3);
	vset(0) = vec2(10,20);
	vset(1) = vec2(20,0);
	vset(2) = vec2(0,0);
}

void Shape::OverrideCalculatedSize(Rect newSize) {
	calculatedPosition = newSize;
}

void Shape::Draw(GL2DProgram * shaders) {
	//Calculate your extents
	float xMin = 1000;
	float xMax = -1000;
	float yMin = 1000;
	float yMax = -1000;
	//calculate limits
	for (int i = 0; i < vertexCount; i++) {
		xMin = min(xMin,vat(i).x);
		xMax = max(xMax,vat(i).x);
		yMin = min(yMin,vat(i).y);
		yMax = max(yMax,vat(i).y);
	}

	//Enable pure-color no texture
	shaders->SetColor(color);
	shaders->EnableTexture(false);

	shaders->Model.PushMatrix();
	//Draw the shape stretched to fit within calculatedPosition
	//Move the shape into the center of the position
	/*shaders->Model.Translate(calculatedPosition.X+calculatedPosition.Width/2.0-(xMax-xMin)/2.0,
							calculatedPosition.Y+calculatedPosition.Height/2.0-(yMax-yMin)/2.0,0);*/
	shaders->Model.Scale(calculatedPosition.Width/(xMax-xMin), calculatedPosition.Height/(yMax-yMin),0);
	shaders->Model.Apply();
	GL2DVertexGroup::Draw();
	shaders->Model.PopMatrix();
}

vec2 & Shape::vset(int i) {
	return vat(i);
}