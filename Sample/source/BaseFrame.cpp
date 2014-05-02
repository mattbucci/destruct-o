#include "stdafx.h"
#include "BaseFrame.h"
#include "GL2DProgram.h"
#include "GL3DProgram.h"
#include "GLTerrainProgram.h"
#include "GLParticleProgram.h"
#include "GLEffectProgram.h"
#include "MaterialProgram.h"
#include "ShaderGroup.h"
#include "ActorPlayer.h"
#include "VoxEngine.h"
#include "Network.h"
#include "GLLighting.h"
#include "Frames.h"
#include "SimplePolygon.h"

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

#include "ParticleSystem.h"
#include "ParticleData.h"

#include "Building.h"
#include "ModelGroup.h"

#include "GLTextureCubeMap.h"

#define MAX_DRAW_DISTANCE 300.0f
#define MIN_DRAW_DISTANCE 30.0f

//Make baseframe a singleton now
BaseFrame * BaseFrame::instance = NULL;

BaseFrame::BaseFrame(ShaderGroup * shaders)
    : GameSystem(shaders), hud(this), Physics(&Voxels), Actors(&Physics), Particles(&Actors,&Physics), achievements(&notification,this)
{
	_ASSERTE(instance == NULL);
	instance = this;

	cout << "\t Constructing base frame\n";
   

    // Remove me test
    cout << "Save directory is: " << OS::SaveDirectory() << endl;
    
	// Create the first person controller depending on the current platform
#ifdef __MOBILE__
	FirstPerson = new FirstPersonModeMobile();
#else
	FirstPerson = new FirstPersonMode();
#endif

	Controls.AddWindow(&notification);
    skybox = new GLSkybox("skybox/skybox", Textures);
    
	//Not a unique save. Should be altered in the future
	SaveName = "Default_Save";
    
	cout << "\t Finished base frame\n";
}

BaseFrame::~BaseFrame()
{

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
    
	// Build player stuff
    Actors.Player()->Build();
    
	return true;
    GameLoaded.Fire([this](function<void(BaseFrame*)> subscriber) {
        subscriber(this);
    });
}

void BaseFrame::Load(Json::Value & parentValue, LoadData & loadData) {
	//This is necessary because FirstPerson is a user owned handle
	//so we must notify the save system we have constructed it
	REPAIR_HANDLE(FirstPerson);
	//Continue loading
	Savable::Load(parentValue,loadData);
    
	//Make any after-load changes
	//Currently no particle systems after load
	Particles.Clear();
}


void BaseFrame::OnFrameFocus() {
	// Enable the first person controller
	FirstPerson->Enable(true);
	GameSystem::OnFrameFocus();
	
}

void BaseFrame::OnFrameLeave() {
	FirstPerson->Enable(false);
	GameSystem::OnFrameLeave();
}

void BaseFrame::NewWorld() {
	//Load the reset save
	Savable::Deserialize(resetSave,this);
	//Build a brave new world or some shit
	Voxels.NewWorld(rand());
	// Build player stuff
    Actors.Player()->Build();
}

void BaseFrame::Build()
{
    // Load the model group from the manifest
    cout << "Loading models\n";
    models = new ModelGroup("meshes/manifest.json", Textures);
    
	//Attempt to load particles
	Particles.Load();

	//Load actors and weapons
	Actors.Load();

	cout << "Loading audio\n";
	audio = new AudioPlayer(100);
    Actors.Player()->Build();
    GameStarted.Fire([this](function<void(BaseFrame*)> subscriber) {
        subscriber(this);
    });
    
   
    // Subscribe to the application state event
    GameEventSubscriber::Subscribe<void (bool)>(&VoxEngine::ApplicationStateChanged, [this](bool applicationActive)
    {
        // If the game is going inactive
        if(!applicationActive)
        {
            // Pause the audio
            audio->Pause();
            
            // Save the game
            this->Save(Game()->GetSaveLocation() + "data.json.compressed");
            
            // Pause
            //pauseWindow->SetVisible(true);
        } else
        {
            // Resume audio
            audio->Resume();
        }
    });
    
    // Build the skybox
    VoxEngine::SynchronousTask.RequestTask([this] ()
    {
        // Build the skybox (upload to GPU)
        skybox->Build();
    });
    
	//Build the reset save
	resetSave = Savable::Serialize(this);
}

bool BaseFrame::Update(vector<InputEvent> inputEvents) {
    
    //Issue events to dialog
	//Run the dialog system and monitor pressed keys
	passEventsToControl(inputEvents);

	// Update the looking direction
	FirstPerson->ReadInput(currentlyPressedKeys,inputEvents);
    
    // Did we want to pause
    if(FirstPerson->GetPauseRequested())
		Frames::SetSystem(Frames::FRAME_PAUSEMENU);
    
    // Update the voxels
	Voxels.Update(Actors.Player()->GetPosition());

	//Update actors
	Actors.Update();

    audio->Update();

	//Update physics/Particles
	Physics.Update();
	Particles.UpdateCloud();

	return true;
}


void BaseFrame::Draw(double width, double height)
{
	// Update the texture caching system
	Textures.Refresh();
    
	// Calculate the view and fog distances from the stored slider
    viewDistance = glm::mix(MIN_DRAW_DISTANCE, MAX_DRAW_DISTANCE, VoxEngine::GlobalSavedData.GameOptions.ViewDistance);
    float fogDistance = viewDistance * 0.9f;
    
    // Compute the view for size vector
	vec2 viewPortSize = vec2((float)width,(float)height);
    
	// Save size in camera as well (for unprojection)
	Camera.UpdateViewSize(viewPortSize);
    Actors.Player()->WeaponCamera().UpdateViewSize(viewPortSize);
    
	// We add 1.5 to ground level. This assumes the person is 5ft between the ground and his eye line
	vec3 pos = Actors.Player()->GetPosition();

	// Setup the camera view for the player
	Camera.SetCameraView(pos, FirstPerson->GetLookVector(), viewDistance);
    Actors.Player()->WeaponCamera().SetCameraView(vec3(0,0,0), glm::vec3(0,-1,0), viewDistance); // BAD BUT TEMPORARY
    
	// Apply properties to each shader
	SetupShader<GLTerrainProgram>("terrain",fogDistance);
	Camera.Apply((GLTerrainProgram*)shaders->GetShader("terrain"));
	SetupShader<GLParticleProgram>("particles",fogDistance);
	SetupShader<GL3DProgram>("3d",fogDistance);
	Camera.Apply((GL3DProgram*)shaders->GetShader("3d"));
    SetupShader<MaterialProgram>("model", fogDistance);
    Camera.Apply((MaterialProgram*)shaders->GetShader("model"));
    SetupShader<GLEffectProgram>("effects", fogDistance);
    Camera.Apply((GLEffectProgram*)shaders->GetShader("effects"));
    
	//Startup 3d rendering
	GL3DProgram * shaders3d = (GL3DProgram*)shaders->GetShader("3d");
	shaders3d->UseProgram();
	shaders3d->Model.Reset();
	shaders3d->Model.Apply();
    
	//Setup sun here for now
	//translate it to follow world coordinates
	shaders3d->Lights.Off();
	shaders3d->Lights.Apply();
    
	//Enable sensible defaults
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Re-enable the depth test
	glEnable(GL_DEPTH_TEST);
    
    // Use the skybox shader
    /*GL3DProgram * skyboxShader = (GL3DProgram *) shaders->GetShader("skybox");
    skyboxShader->UseProgram();
    
    // Setup the skybox's camera
    skyboxShader->Camera.SetCameraPosition(vec3(0,0,0), FirstPerson->GetLookVector());
    skyboxShader->Camera.SetFrustrum(60,viewPortSize.x/viewPortSize.y,.25, viewDistance); //width/height
    skyboxShader->Camera.Apply();
    
    // Apply the model transform (identity transform =/)
    skyboxShader->Model.Apply();
    
    // Draw the skybox
    skybox->Draw(skyboxShader);*/
	
	// Draw the voxels
    SimplePolygon<4> viewArea;
    Camera.GetViewingArea(viewArea);
    Voxels.Draw(shaders, viewArea);
    
	//The physics system uses the same texture that the voxels above binds every time it draws
	//so it must always immediately follow Voxels.draw()
	/*Physics.Draw(shaders);

	//Turn lights back on because I'm not sure if this is necessary
	shaders3d->Lights.On();
	//shaders3d->Lights.Apply();
    
    // Setup the mesh shader boneless
    MaterialProgram * modelShader = (MaterialProgram *) shaders->GetShader("model");
    modelShader->UseProgram();
    modelShader->Lights.Enable(1);
    modelShader->Lights.Apply();
    
    // Draw the actors
	Actors.Draw(shaders);

    // Draw the player weapon
    modelShader->UseProgram();
    modelShader->Lights.Apply();
    
    Actors.Player()->DrawWeapon(modelShader);
    
	//The particle system will use a different shader entirely soon
	Particles.Draw(shaders);*/
	
	//Update the voxel debug counter
	Controls.Debug.Voxels = Voxels.GetLastVoxelCount();

	//Set up the 2D Shader
	GL2DProgram * shaders2d = SetWidthHeight(viewPortSize.x,viewPortSize.y);
    
	//Enable sensible defaults
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Draw the UI for joysticks
	FirstPerson->Draw(width, height, shaders2d);
    
	//draw the hud
	hud.DrawAndUpdate(shaders2d,viewPortSize);

    //Call the parent draw to draw interface
	GameSystem::Draw(width,height);
} 

void BaseFrame::PushNotification(string txt) {
	notification.Notify(txt);
}

ModelGroup* BaseFrame::Models() 
{
    return models;
}

string BaseFrame::GetSaveLocation() {
	return OS::SaveDirectory() + "Saves/" + SaveName + "/";
}

//Retrieve the game object
//global function
BaseFrame * Game() {
	_ASSERTE(BaseFrame::instance != NULL);
	return BaseFrame::instance;
}

HUD* BaseFrame::GetHUD() {
	return &hud;
}

// As we have the skybox, we only need clear the depth buffer
GLenum BaseFrame::ClearBits()
{
    return GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
}
