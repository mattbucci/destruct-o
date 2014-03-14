#include "stdafx.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "VoxEngine.h"
#include "DesignFrame.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "Structure.h"
#include "MaterialSelectionControl.h"

#include "OS.h"

DesignFrame::DesignFrame(ShaderGroup * shaders) : GameSystem(shaders), 
	//Setup the editor with a connection to the camera
	editor(&Camera) {
	//Move this into a load section
	toEdit = new Structure();
	toEdit->Extents = vec3(10,10,5);

	editor.EditStructure(toEdit);
	Camera.SetCameraView(toEdit->Extents*.5f);
	mouseDown = false;
	panning = false;


	matWindow.position = Rect(0,0,200,200);
	matWindow.hPin = Control::MAX;
	matWindow.vPin = Control::CENTER;
	matSelector = new MaterialSelectionControl();
	matSelector->position = Rect(10,10,180,180);
	matWindow.AddControl(matSelector);
	Controls.AddWindow(&matWindow);

	fileMenu = new FileMenuWindow(&editor);
	Controls.AddWindow(fileMenu);
}
DesignFrame::~DesignFrame() {

}

void DesignFrame::OnFrameFocus() {

}

void DesignFrame::Build() {

}

bool DesignFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Issue events to dialog
	//Run the dialog system and monitor pressed keys
	//events read by the dialog system are removed from the list of events
	passEventsToControl(inputEvents);

	//Read events and apply to camera rotation
	for (auto e : inputEvents) {
		if (e.Event == InputEvent::MouseDown) {
			mouseDown = true;
			prevMousePos = vec2(e.MouseX,e.MouseY);
			mouseHeldAt = OS::Now();
		}
		else if (e.Event == InputEvent::MouseUp) {
			if (!panning) {
				//The user clicked this position
				editor.SetMaterial(matSelector->GetSelectedMaterial());
				editor.PlaceVoxel();
			}
			panning = false;
			mouseDown = false;
		}
		else if (e.Event == InputEvent::MouseMove) {
			if (mouseDown) {
				vec2 cur = vec2(e.MouseX,e.MouseY);
				vec2 diff = cur - prevMousePos;

				if (panning) {
					//Apply rotation in degrees (1 degree per pixel of mouse movement)
					Camera.Pan += diff.x;
					Camera.Tilt += diff.y;
					//Save the new mouse pos
					prevMousePos = cur;
				}
				else {
					//If the user holds down click for a long time, or moves the mouse around
					//assume they want to be panning
					if ((OS::Now() - mouseHeldAt) > .3)
						panning = true;
					else if (glm::length(cur - prevMousePos) > 15)
						panning = true;
				}

			}
		}
		else if (e.Event == InputEvent::MouseScroll) {
			Camera.Distance += e.Key*5;
			if (Camera.Distance > 100)
				Camera.Distance = 100;
			if (Camera.Distance < 5)
				Camera.Distance = 5;
		}
	}

	//Pass events to the editor if the user isn't holding the mouse
	if (!mouseDown)
		editor.ReadInput(inputEvents);

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
	shaders3d->Fog.SetFogDistance(5000.0f);
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Apply the camera
	Camera.Draw(shaders3d);

	//Draw the editor
	editor.Draw(shaders3d);

	//Call the parent draw to draw interface
	GameSystem::Draw(width,height);

} 
