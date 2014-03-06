#include "stdafx.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "VoxEngine.h"
#include "DesignFrame.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

DesignFrame::DesignFrame(ShaderGroup * shaders) : GameSystem(shaders) {

}
DesignFrame::~DesignFrame() {

}

void DesignFrame::OnFrameFocus() {

}

void DesignFrame::Build() {

}

bool DesignFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {

	return true;
}

void DesignFrame::Draw(double width, double height) {
	vec2 viewPortSize = vec2((float)width,(float)height);
	//Save size in camera as well (for unprojection)
	Camera.UpdateViewSize(viewPortSize);

	//Update the texture caching system
	Textures.Refresh();

	//Startup 3d rendering
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	shaders3d->UseProgram();
	//Setup basics of shader program per-frame
	shaders3d->Model.Reset();
	shaders3d->Model.Apply();
	shaders3d->Acid.SetCurrentTime(VoxEngine::GetGameSimTime());
	//Acid factor currently managed by the demo system
	//this will be moved to a more powerful game logic system in the future
	shaders3d->Acid.SetAcidFactor(0.0);
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	//Call the parent draw to draw interface
	GameSystem::Draw(width,height);
	/*
	// Draw the UI for joysticks
	GL2DProgram * shaders2d = (GL2DProgram*)shaders->GetShader("2d");
	shaders2d->UseProgram();
	shaders2d->SetWidthHeight((float)width, (float)height);
	FirstPerson->Draw(width, height, shaders2d);
	*/
} 