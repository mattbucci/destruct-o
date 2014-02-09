#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

BaseFrame::BaseFrame(ShaderGroup * shaders) : GameSystem(shaders) {
	cout << "\t Constructing base frame\n";
	//Load the shaders appropriate for the opengl version being used
	if (OpenglVersion == 31) {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram("Interface/Shaders/glsl31/vsh_interface.glsl","Interface/Shaders/glsl31/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		shaders->AddShader(shaders2d,"2d");
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram("Interface/Shaders/glsl31/vsh_3d.glsl","Interface/Shaders/glsl31/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		shaders->AddShader(shaders3d,"3d");
	}
	else {
		//Build the dialog shader
		GL2DProgram * shaders2d = new GL2DProgram("Interface/Shaders/glsl11/vsh_interface.glsl","Interface/Shaders/glsl11/fsh_interface.glsl");
		if (!shaders2d->Valid()) 
			cout << "Failed to build opengl program\n";
		shaders->AddShader(shaders2d,"2d");
		//Build the voxel shader
		GL3DProgram * shaders3d = new GL3DProgram("Interface/Shaders/glsl11/vsh_3d.glsl","Interface/Shaders/glsl11/fsh_3d.glsl");
		if (!shaders3d->Valid()) 
			cout << "Failed to build opengl program\n";
		shaders->AddShader(shaders3d,"3d");
	}


	//Build the sample dialog 
	//Build a window that says "On Top"
	Window * window = new Window(Rect(0,0,200,400),"On Bottom");
	window->hPin = Control::CENTER;
	window->vPin = Control::MAX;
	window->SetVisible(false);
	Controls.AddWindow(window);
	//Give it a list box with a bunch of random crap
	vector<string> randomCrap;
	randomCrap.push_back("Monkey");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Cheeto");
	randomCrap.push_back("Spaghetii");
	randomCrap.push_back("Canada");
	randomCrap.push_back("Banana");
	randomCrap.push_back("One");
	randomCrap.push_back("Seventeen");
	randomCrap.push_back("Lamp");
	randomCrap.push_back("Voxel");
	randomCrap.push_back("Game");
	randomCrap.push_back("Walrus");
	randomCrap.push_back("Still Walrus");
	randomCrap.push_back("Cloud");
	Listbox * list = new Listbox();
	list->position = Rect(0,0,180,150);
	list->hPin = list->vPin = Control::CENTER;
	list->SetEntries(randomCrap);
	//When the user selects something, change the title to the 
	//selected item
	Subscribe<void(Listbox*,int)>(&list->EventSelectionChanged, [window](Listbox* list,int changedTo) {
		if (changedTo >= 0)
			window->SetTitle(list->GetEntries()[changedTo]);
	});
	window->AddControl(list);

	//Start another window (visible by default)
	Window * wm = new Window(Rect(0,0,300,300),"HELLO =)");
	wm->hPin = wm->vPin = Control::CENTER;
	Controls.AddWindow(wm);
	//Add a button to this window that when pressed
	//switches which window is visible
	Button * windowButton = new Button(Rect(0,-10,100,30),"PRESS ME");
	windowButton->hPin = Control::CENTER;
	windowButton->vPin = Control::MAX;
	Subscribe<void(Button*)>(&windowButton->EventClicked,[window,wm](Button* button) {
		//Make the current window invisible
		wm->SetVisible(false);
		//Make the next window visible
		window->SetVisible(true);
	});
	wm->AddControl(windowButton);
	
	cout << "\t Finished base frame\n";
}
BaseFrame::~BaseFrame() {

}

void BaseFrame::Build() {
	//Load the sample tile
	if (!Voxels.LoadTile("basic-h.png")) {
		cout << "Failed to load voxel tile\n";
	}
}

bool BaseFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Issue events to dialog
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	return true;
}

void BaseFrame::Draw(double width, double height) {
	vec2 viewPortSize = vec2((float)width,(float)height);
	//Save size in camera as well (for unprojection)
	Camera.UpdateViewSize(viewPortSize);
	vec2 mapExtents = vec2(Voxels.GetWidth(),Voxels.GetHeight());

	//Rotate camera
	//Camera.Rotation((float)((OS::Now()/30.0)*2.0f*M_PI));


	//Startup 3d rendering
	//Enable the 2d shader for interface drawing
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	shaders3d->UseProgram();
	shaders3d->Model.Reset();
	shaders3d->Model.Apply();
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Compute view distance and handle fog
	ViewDistance.CalculateAndApply(shaders3d,fpsCount.GetFps());
	shaders3d->Fog.SetFogColor(vec4(.5,.5,.5,1.0));
	//Calculate voxel draw rectangle
	/*pair<vec2,vec2> drawRectangle = viewDistance.VoxDrawCoordinates(viewPortSize,mapExtents,vec2(camera.Position()),camera.Rotation());
	vec2 minPoint = drawRectangle.first;
	vec2 maxPoint = drawRectangle.second;

	//Draw the frame
	//camera draw also sets up world light
	Camera.Draw(shaders3d);
	Voxels.Draw(shaders3d,camera.Position(),(int)minPoint.x,(int)minPoint.y,(int)maxPoint.x,(int)maxPoint.y);*/
	
	//Update the voxel debug counter
	Controls.Debug.Voxels = Voxels.GetLastVoxelCount();

	//Call the parent draw to draw interface
	GameSystem::Draw(width,height);
}