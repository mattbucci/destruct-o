#include "stdafx.h"
#include "GameSystem.h"
#include "GL2DProgram.h"
#include "ShaderGroup.h"
#include "OS.h"
#include "VoxEngine.h"

GameSystem::GameSystem(ShaderGroup * _shaders)
    : shaders(_shaders)
{
	nextFrameTime = 0;
	fps = 40;
	simTime = 0;
	isReady = false;
}

GameSystem::~GameSystem() {

}

void GameSystem::passEventsToControl(vector<InputEvent> & inputEvents) {
	//Build a new list of events not used by the dialog system
	vector<InputEvent> nonDialogInputs;
	//Process key events
	for (InputEvent & event : inputEvents) {
		bool used = Controls.FireEvent(event);

		if (!used)
			nonDialogInputs.push_back(event);

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

	inputEvents = nonDialogInputs;
}


bool GameSystem::Update(vector<InputEvent> inputEvents) {
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	return true;
}

//By default only runs the dialog system
void GameSystem::Draw(double width, double height) {

	GL2DProgram * shaders2d = (GL2DProgram*)shaders->GetShader("2d");

	//Update debug data before draw
	Controls.Debug.Fps = fpsCount.GetFps();
	//and profiling debug data
	Controls.Debug.UpdateTime = VoxEngine::UpdateTime.GetAverage();
	Controls.Debug.DrawTime = VoxEngine::DrawTime.GetAverage();

	//Draw the window
	//Controls.Draw(shaders2d,Rect(0,0,(float)width,(float)height));

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
	//Clear out held keys
	currentlyPressedKeys.clear();
}

//Set the Width and Height of Frame
GL2DProgram * GameSystem::SetWidthHeight(float width, float height) {
	GL2DProgram * shaders2d = (GL2DProgram*)shaders->GetShader("2d");
	shaders2d->UseProgram();
	shaders2d->SetWidthHeight((float)width,(float)height);
	return shaders2d;
}

double GameSystem::Now() {
	return simTime;
}

//Function to test if GameSystem is Ready
bool GameSystem::IsReady() {
	return isReady;
}

// Standard clear bits
GLenum GameSystem::ClearBits()
{
    return GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
}