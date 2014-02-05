#include "stdafx.h"
#include "DebugWindow.h"
#include "Label.h"
#include "Utilities.h"

DebugWindow::DebugWindow() {
	this->position = Rect(0,0,120,50);

	fps = new Label(0,5,"");
	fps->hPin = Control::CENTER;
	this->AddChild(fps);
	voxels = new Label(0,20,"");
	voxels->hPin = Control::CENTER;
	this->AddChild(voxels);
}
	
void DebugWindow::Draw(GL2DProgram * shader) {
	if (Fps >= 0) {
		fps->SetText(string("FPS: ") + Utilities::toString((int)Fps));
		fps->SetVisible(true);
	}
	else
		fps->SetVisible(false);

	if (Voxels >= 0) {
		voxels->SetText(string("Voxels: ") + Utilities::toString(Voxels));
		voxels->SetVisible(true);
	}
	else
		voxels->SetVisible(false);
	
	
	Window::Draw(shader);
}