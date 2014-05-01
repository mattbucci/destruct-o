#include "stdafx.h"
#include "PauseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "OS.h"

#include "PauseWindow.h"
#include "PauseWindowMobile.h"


PauseFrame::PauseFrame(ShaderGroup * shaders)
    : GameSystem(shaders), background(Rect(0, 0, 800, 600),"menu/background.png", vec4(1,1,1,1)) 
{
#ifdef __MOBILE__
	pauseWindow = new PauseWindowMobile();
#else
	pauseWindow = new PauseWindow();
#endif
	Controls.AddWindow(pauseWindow);
}

PauseFrame::~PauseFrame()
{

}

void PauseFrame::OnFrameFocus() {
}



bool PauseFrame::Update(vector<InputEvent> inputEvents) {
	//Pass Update to GameSystem to Handle Control Input
	return GameSystem::Update(inputEvents);
}


void PauseFrame::Draw(double width, double height)
{
	GL2DProgram * shaders2d = SetWidthHeight((float)width, (float)height);

	shaders2d->Model.Reset();
	shaders2d->Model.Apply();
	background.Draw(shaders2d);
	shaders2d->Model.Translate((float)(width/2.0f - 648/2.0f), 80, 0);
	shaders2d->Model.Apply();
	shaders2d->Model.Reset();
	shaders2d->Model.Apply();

	//Call the parent draw to draw interface
	GameSystem::Draw(width, height);
}

