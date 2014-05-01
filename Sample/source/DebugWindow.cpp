#include "stdafx.h"
#include "DebugWindow.h"
#include "Label.h"
#include "Utilities.h"

DebugWindow::DebugWindow() {
#ifdef __MOBILE__
	this->position = Rect(-95,20,120,70);
	this->hPin = Control::MAX;
#else
	this->position = Rect(0,20,120,70);
#endif
	color = vec4(1,1,1,.66);
	fps = new Label(0,2,"");
	fps->hPin = Control::CENTER;
	this->AddChild(fps);
	voxels = new Label(0,17,"");
	voxels->hPin = Control::CENTER;
	this->AddChild(voxels);
	drawTime = new Label(0,32,"");
	drawTime->hPin = Control::CENTER;
	this->AddChild(drawTime);
	updateTime = new Label(0,47,"");
	updateTime->hPin = Control::CENTER;
	this->AddChild(updateTime);

	takeInput = false;
	

	Fps = DrawTime = UpdateTime = 0.0f;
	Voxels = 0;
	curDrawCycle = 0;
}
	
void DebugWindow::Draw(GL2DProgram * shader) {
	curDrawCycle++;

	if ((curDrawCycle % 5) == 0) {
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
		

		if (DrawTime >= 0) {
			drawTime->SetText(string("Draw: ") + Utilities::toString(floor(DrawTime*1000)/1000));
			drawTime->SetVisible(true);
		}
		else
			drawTime->SetVisible(false);
		
	
		if (UpdateTime >= 0) {
			updateTime->SetText(string("Update: ") + Utilities::toString(floor(UpdateTime*1000)/1000));
			updateTime->SetVisible(true);
		}
		else
			updateTime->SetVisible(false);
		
	}
	Window::Draw(shader);
}