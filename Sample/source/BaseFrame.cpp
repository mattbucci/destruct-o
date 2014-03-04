#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "ActorPlayer.h"
#include "VoxEngine.h"
#include "GLMesh.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

#include "ParticleSystem.h"
#include "ParticleData.h"

#include "Demo.h"

BaseFrame::BaseFrame(ShaderGroup * shaders) : GameSystem(shaders), Physics(&Voxels) {
	cout << "\t Constructing base frame\n";

	
	// Get the 3D shader program
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	uniformModelView = glGetUniformLocation(shaders3d->GetId(), "MV");
	uniformModelViewProjection = glGetUniformLocation(shaders3d->GetId(), "MVP");

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
	//Controls.AddWindow(wm);
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
	
	// Create the first person controller depending on the current platform
#ifdef __MOBILE__
	FirstPerson = new FirstPersonModeMobile();
#else
	FirstPerson = new FirstPersonMode();
#endif
	
	// Enable the first person controller
	FirstPerson->Enable(true);
    
    // Test load a mesh
    GLMesh mesh("meshes/phoenix_ugv.md2");
	
	cout << "\t Finished base frame\n";
	//testSystem = NULL;
}
BaseFrame::~BaseFrame() {

}

void BaseFrame::OnFrameFocus() {
	//Build actors, right now just the player
	player = new ActorPlayer();
	//The player autoregisters himself with the actor system
	//we do not need to do that by hand

	//The physics demo
	//we won't have this forever
	demo = new Demo();
#ifdef __MOBILE__
	demoWindow = new DemoWindow(demo);
	Controls.AddWindow(demoWindow);
#endif
}

void BaseFrame::Build() {
	//load the audio
	audio = new AudioPlayer(100);
	audio->Subscribe(player);
	//Load the sample tile
	if (!Voxels.LoadTile("basic-h.png")) {
		cout << "Failed to load voxel tile\n";
	}
}

bool BaseFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Issue events to dialog
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	//Update the looking direction
	FirstPerson->ReadInput(currentlyPressedKeys,inputEvents);

	//The player is the only actor which reads input
	//player->ReadInput(inputEvents);

	//Update actors
	Actors.Update(delta,now);

	demo->OnInput(inputEvents,player->GetPosition(),FirstPerson->GetLookVector());
	demo->Update(now,delta);

	//Update physics/Particles
	Physics.Update(delta,now);
	Particles.UpdateCloud(now,delta);
	//if (testSystem != NULL)
		//testSystem->UpdateEmitter(now);

	//Update demo
	demo->CheckTouchInput(player->GetPosition(),FirstPerson->GetLookVector());
	return true;
}

void BaseFrame::Draw(double width, double height) {
	vec2 viewPortSize = vec2((float)width,(float)height);
	//Save size in camera as well (for unprojection)
	Camera.UpdateViewSize(viewPortSize);
	vec2 mapExtents = vec2(Voxels.GetWidth(),Voxels.GetHeight());

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
	shaders3d->Acid.SetAcidFactor(demo->CurrentAcidStrength);
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Compute view distance and handle fog
	ViewDistance.CalculateAndApply(shaders3d,fpsCount.GetFps());
	shaders3d->Fog.SetFogColor(vec4(.5,.5,.5,1.0));

	//We add 1.5 to ground level. This assumes the person is 5ft between the ground
	//and his eye line
	vec3 pos = player->GetPosition();
	//The player is 3 height right now
	pos.z += 2.5;
	//Calculate voxel draw rectangle
	pair<vec2,vec2> drawRectangle = ViewDistance.VoxDrawCoordinates(viewPortSize,mapExtents,vec2(pos),FirstPerson->GetAngleVector().x/180.0f*(float)M_PI);
	vec2 minPoint = drawRectangle.first;
	vec2 maxPoint = drawRectangle.second;

	//Draw the frame
	//camera draw also sets up world light
	Camera.SetCameraView(pos,FirstPerson->GetLookVector());
	Camera.Draw(shaders3d);
	
	// Draw voxels
	Voxels.Draw(shaders3d,pos,(int)minPoint.x,(int)minPoint.y,(int)maxPoint.x,(int)maxPoint.y);
	//The physics system uses the same texture that the voxels above binds every time it draws
	//so it must always immediately follow Voxels.draw()
	Physics.Draw(shaders);

	//The particle system will use a different shader entirely soon
	Particles.Draw(shaders);
	
	//Update the voxel debug counter
	Controls.Debug.Voxels = Voxels.GetLastVoxelCount();
	
	//Call the parent draw to draw interface
	GameSystem::Draw(width,height);
	
	// Draw the UI for joysticks
	//Assume that gameSystem::Draw has set up the 2d shader
	GL2DProgram * shaders2d = (GL2DProgram*)shaders->GetShader("2d");
	FirstPerson->Draw(width, height, shaders2d);
} 