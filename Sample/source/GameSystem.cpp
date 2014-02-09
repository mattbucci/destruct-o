#include "stdafx.h"
#include "GameSystem.h"
#include "GL2DProgram.h"
#include "ShaderGroup.h"
#include "OS.h"

GameSystem::GameSystem(ShaderGroup * shaders) {
	this->shaders = shaders;
	nextFrameTime = 0;
	fps = 40;
}

GameSystem::~GameSystem() {

}

void GameSystem::Build() {

}

void GameSystem::passEventsToControl(vector<InputEvent> inputEvents) {
	//Process key events
	for (InputEvent & event : inputEvents) {
		bool used = Controls.FireEvent(event);

		if (event.Event == InputEvent::KeyboardUp) {
			//Remove the key if its in the pressed keys set
			auto at = currentlyPressedKeys.find(event.Key);
			if (at != currentlyPressedKeys.end())
				currentlyPressedKeys.erase(at);
		}
		else if (!used){
			//Add the key if its not in the pressed keys set
			auto at = currentlyPressedKeys.find(event.Key);
			if (at == currentlyPressedKeys.end())
				currentlyPressedKeys.insert(event.Key);
		}
	}
}


bool GameSystem::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	//Handle frame drawing
	//Maintain a constant fps for now
	//this is changed to fix update, variable draw when certain frames are running
	OS::WaitUntil(nextFrameTime);
	nextFrameTime = OS::Now()+1.0/(double)fps;

	return true;
}

//By default only runs the dialog system
void GameSystem::Draw(double width, double height) {

	//Enable the 2d shader for interface drawing
	GL2DProgram * shaders2d = (GL2DProgram*)shaders->GetShader("2d");
	shaders2d->UseProgram();
	shaders2d->SetWidthHeight((float)width,(float)height);
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Update debug data before draw
	Controls.Debug.Fps = fpsCount.GetFps();

	//Draw the window
	Controls.Draw(shaders2d,Rect(0,0,(float)width,(float)height));

	//Keep track of fps
	fpsCount.MarkFrameRendered();//*/
	
}

//When this frame starts running this will be called
void GameSystem::OnFrameFocus() {

}
//When this frame stops running this will be called
void GameSystem::OnFrameLeave() {
	//Send the leave event to the controls
	InputEvent ie(InputEvent::MouseMove,0,-1,-1);
	Controls.FireEvent(ie);
}