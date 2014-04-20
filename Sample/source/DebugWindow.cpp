#include "stdafx.h"
#include "DebugWindow.h"
#include "Label.h"
#include "Utilities.h"

DebugWindow::DebugWindow() {
#ifdef __MOBILE__
	this->position = Rect(-95,20,120,40);
	this->hPin = Control::MAX;
#else
	this->position = Rect(0,20,120,40);
#endif
	color = vec4(1,1,1,.66);
	fps = new Label(0,2,"");
	fps->hPin = Control::CENTER;
	this->AddChild(fps);
	voxels = new Label(0,17,"");
	voxels->hPin = Control::CENTER;
	takeInput = false;
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