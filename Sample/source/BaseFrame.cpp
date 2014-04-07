#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "GLTerrainProgram.h"
#include "GLParticleProgram.h"
#include "ShaderGroup.h"
#include "ActorPlayer.h"
#include "VoxEngine.h"

#include "GLLighting.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

#include "ParticleSystem.h"
#include "ParticleData.h"

#include "Demo.h"
#include "Building.h"

BaseFrame::BaseFrame(ShaderGroup * shaders) : GameSystem(shaders), Physics(&Voxels) {
	cout << "\t Constructing base frame\n";
		
	// Create the first person controller depending on the current platform
#ifdef __MOBILE__
	FirstPerson = new FirstPersonModeMobile();
#else
	FirstPerson = new FirstPersonMode();
#endif
	
	// Enable the first person controller
	FirstPerson->Enable(true);
	
	cout << "\t Finished base frame\n";
	//testSystem = NULL;
}
BaseFrame::~BaseFrame() {

}


//synchronously saves the game
bool BaseFrame::Save(string saveFile) {
	//Serialize this class
	vector<unsigned char> saveData = Savable::Serialize(this);

	//Open for writing binary data
	SDL_RWops *file = SDL_RWFromFile(saveFile.c_str(), "wb"); 

	//Check the file opened
	if(!file) 
		return false;
	//Write the file containing all the save data
	SDL_RWwrite(file,(char*)&saveData.front(),1,saveData.size());
	SDL_RWclose(file);

	return true;
}
//synchronously loads the game over any existing data
bool BaseFrame::Load(string saveFile) {
	SDL_RWops *file = SDL_RWFromFile(saveFile.c_str(), "rb"); 
	long size;
	
	//Check the file opened
	if(!file) 
		return false;

	//Determine file size
	SDL_RWseek(file , 0 , RW_SEEK_END);
	size = (long)SDL_RWtell(file);
	SDL_RWseek(file,0,RW_SEEK_SET);

	//allocate space for file data
	vector<unsigned char> fileData(size);
	SDL_RWread(file,&fileData.front(), 1, (size_t)size);
	SDL_RWclose(file);
	
	//Deserialize the data
	Savable::Deserialize(fileData,this);

	return 0;
}

void BaseFrame::Load(Json::Value & parentValue, LoadData & loadData) {
	//This is necessary because FirstPerson is a user owned handle
	//so we must notify the save system we have constructed it
	REPAIR_HANDLE(FirstPerson);
	//Continue loading
	Savable::Load(parentValue,loadData);
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
	notification = Notification::init();
	Controls.AddWindow(notification);
}

void BaseFrame::Build() {

	//load the audio
	cout << "Loading audio\n";
	audio = new AudioPlayer(100);
	audio->Subscribe(player);
	
}

bool BaseFrame::Update(double delta,double now, vector<InputEvent> inputEvents) {
	//Issue events to dialog
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	//Update the looking direction
	FirstPerson->ReadInput(currentlyPressedKeys,inputEvents);

	//The player is the only actor which reads input
	//player->ReadInput(inputEvents);

	Voxels.Update(player->GetPosition());

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

	//Update the texture caching system
	Textures.Refresh();

	float fogDistance = 0;
	//Apply view distance
	ViewDistance.CalculateAndApply(fogDistance,fpsCount.GetFps());


	//We add 1.5 to ground level. This assumes the person is 5ft between the ground
	//and his eye line
	vec3 pos = player->GetPosition();
	//The player is 3 height right now
	pos.z += 2.5;


	//Draw the frame
	//camera draw also sets up world light
	Camera.SetCameraView(pos,FirstPerson->GetLookVector());

	//Apply properties to each shader
	SetupShader<GLTerrainProgram>("terrain",fogDistance);
	Camera.Apply((GLTerrainProgram*)shaders->GetShader("terrain"));
	SetupShader<GLParticleProgram>("particles",fogDistance);
	SetupShader<GL3DProgram>("3d",fogDistance);
	Camera.Apply((GL3DProgram*)shaders->GetShader("3d"));

	//Startup 3d rendering
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	shaders3d->UseProgram();
	//Setup sun here for now
	//translate it to follow world coordinates
	shaders3d->Lights.Enable(1);
	shaders3d->Lights[0].SetColor(vec3(1,1,1));
	shaders3d->Lights[0].SetIntensity(0,0,0); //The sun shines infinitely long
	shaders3d->Lights[0].SetComponents(.4f,1,1,20);
	shaders3d->Lights[0].SetLocation(shaders3d->Model.TransformVector(vec3(-200000,-200000,400000)));
	shaders3d->Lights.Apply();
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	// Draw voxels
	Voxels.Draw(shaders,pos,ViewDistance.GetDrawRegion(vec2(pos),FirstPerson->GetAngleVector().x/180.0f*(float)M_PI));
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