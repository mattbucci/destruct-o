#include "stdafx.h"
#include "MaterialSelectionControl.h"
#include "GL2DProgram.h"
#include "Frames.h"
#include "GameSystem.h"


MaterialSelectionControl::MaterialSelectionControl() :
	selectionShape(4) {
	selectionShape.SetToSquare();
	currentlySelectedMaterial = 0;
}
MaterialSelectionControl::MaterialSelectionControl(Rect position) :
	selectionShape(4)  {
	selectionShape.SetToSquare();
	currentlySelectedMaterial = 0;
	this->position = position;
}
MaterialSelectionControl::~MaterialSelectionControl() {

}


void MaterialSelectionControl::OnMousePress(vec2 mousePos, int button, bool down) {
	//Determine the mouse position
	int tileX = floor(mousePos.x/position.Width*4);
	int tileY = floor(mousePos.y/position.Height*4);
	currentlySelectedMaterial = tileX + tileY*4;
}

void MaterialSelectionControl::Draw(GL2DProgram * shaders) {
	shaders->Model.Apply();
	//Draw the classic square
	vat(0) = vec2(0,0);
	xat(0) = vec2(0,0);
	vat(1) = vec2(0,calculatedPosition.Height);
	xat(1) = vec2(0,1);
	vat(2) = vec2(calculatedPosition.Width,0);
	xat(2) = vec2(1,0);
	vat(3) = vec2(calculatedPosition.Width,calculatedPosition.Height);	
	xat(3) = vec2(1,1);
	//Enable the texture
	CurrentSystem->Textures.GetTexture("terrain/tiles-lowres.png")->Bind();
	shaders->SetColor(vec4(1,1,1,1));
	shaders->EnableTexture(true);

	//Draw the square
	GL2DVertexGroup::Draw(shaders);

	//Draw the selection shape here instead of as a child
	if (currentlySelectedMaterial >= 0) {
		Rect pos = Rect((currentlySelectedMaterial % 4)*1.0f/4.0f*position.Width,
			(int)(currentlySelectedMaterial / 4)*1.0f/4.0f*position.Height
			,position.Width/4.0f-20,position.Height/4.0f-20);

		shaders->Model.PushMatrix();
		shaders->Model.Translate(pos.X+10,pos.Y+10,0);
		shaders->Model.Apply();
		selectionShape.color = vec4(1,1,1,.5);
		selectionShape.OverrideCalculatedSize(pos);
		selectionShape.Draw(shaders);
		shaders->Model.PopMatrix();
	}
}

int MaterialSelectionControl::GetSelectedMaterial() {
	return currentlySelectedMaterial;
}