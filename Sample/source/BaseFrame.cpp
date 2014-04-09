#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "ActorPlayer.h"
#include "VoxEngine.h"

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
    
    // Test load a mesh
    mesh[0] = new GLMesh("meshes/robot02",         "robot02.mesh.json",         Textures, vec3(1.500, 1.500, 1.500));
    mesh[1] = new GLMesh("meshes/scifi_soldier01", "scifi_soldier01.mesh.json", Textures, vec3(0.300, 0.300, 0.300));
    mesh[2] = new GLMesh("meshes/scifi_soldier02", "scifi_soldier02.mesh.json", Textures, vec3(0.015, 0.015, 0.015));
	
	cout << "\t Finished base frame\n";
	//testSystem = NULL;
}
BaseFrame::~BaseFrame()
{
    delete mesh[0];
    delete mesh[1];
    delete mesh[2];
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

void BaseFrame::Build()
{
    // Test load a mesh
    /*mesh[0] = new GLMesh("meshes/robot02",         "robot02.mesh.json",              Textures, vec3(1.5,   1.5,   1.5));
    mesh[1] = new GLMesh("meshes/scifi_soldier02", "scifi_soldier02.mesh.json",      Textures, vec3(0.015, 0.015, 0.015));
    mesh[2] = new GLMesh("meshes/scifi_soldier02", "scifi_soldier02_dark.mesh.json", Textures, vec3(0.015, 0.015, 0.015));*/
    
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
	shaders3d->Fog.SetFogColor(vec4(.5,.5,.5,1));

	//We add 1.5 to ground level. This assumes the person is 5ft between the ground
	//and his eye line
	vec3 pos = player->GetPosition();
	//The player is 3 height right now
	pos.z += 2.5;
	//Calculate voxel draw rectangle
	pair<vec2,vec2> drawRectangle = ViewDistance.VoxDrawCoordinates(viewPortSize,vec2(pos),FirstPerson->GetAngleVector().x/180.0f*(float)M_PI);
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
    
    // Setup the mesh shader
    GL3DProgram * shadersMesh = (GL3DProgram *)shaders->GetShader("mesh");
    shadersMesh->UseProgram();
    shadersMesh->Acid.SetCurrentTime(VoxEngine::GetGameSimTime());
    shadersMesh->Acid.SetAcidFactor(demo->CurrentAcidStrength);
    shadersMesh->Fog.SetFogColor(vec4(.5, .5, .5, 1.0));
    shadersMesh->Fog.SetFogDistance(shaders3d->Fog.GetFogDistance());
    Camera.Draw(shadersMesh);
    
    // Draw the meshes
    for(std::vector<std::pair<glm::vec3, int> >::iterator it = Meshes.begin(); it != Meshes.end(); ++it)
    {
        // Set the proper translation
        shadersMesh->Model.PushMatrix();
		shadersMesh->Model.Translate((*it).first);
        shadersMesh->Model.PushMatrix();
        shadersMesh->Model.Rotate(90.0, 1, 0, 0);
		shadersMesh->Model.Apply();
        
        // Draw the mesh
        mesh[(*it).second]->Draw(shadersMesh);
        
        // Remove this translation
        shadersMesh->Model.PopMatrix();
        shadersMesh->Model.PopMatrix();
    }

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