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

#include "Window.h"
#include "Button.h"
#include "Listbox.h"

#include "OS.h"

#include "ParticleSystem.h"
#include "ParticleData.h"

#include "Demo.h"
#include "Building.h"

#include "GLTextureCubeMap.h"

BaseFrame::BaseFrame(ShaderGroup * shaders)
    : GameSystem(shaders), 
	Physics(&Voxels), 
	Actors(&Physics),
	Particles(&Actors,&Physics),
	achievements(&notification,this),
	hud(this)
{
	cout << "\t Constructing base frame\n";
    
    // Remove me test
    cout << "Save directory is: " << OS::SaveDirectory() << endl;
    
	// Create the first person controller depending on the current platform
#ifdef __MOBILE__
	FirstPerson = new FirstPersonModeMobile();
	pauseWindow = new PauseWindowMobile(this);
#else
	FirstPerson = new FirstPersonMode();
	pauseWindow = new PauseWindow(this);
#endif

	Controls.AddWindow(pauseWindow);
	Controls.AddWindow(&notification);

	//Not a unique save. Should be altered in the future
	SaveName = "Default_Save";
    
	cout << "\t Finished base frame\n";
}

BaseFrame::~BaseFrame()
{

}


void BaseFrame::ToggleMenu() {
	if(pauseWindow->toggle()) {
		FirstPerson->Enable(false);
	} else {
		FirstPerson->Enable(true);
	}
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

#include "ActorAI.h"

void BaseFrame::OnFrameFocus() {
	// Enable the first person controller
	FirstPerson->Enable(true);

	
}

void BaseFrame::OnFrameLeave() {
	FirstPerson->Enable(false);
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
	Particles.LoadParticles();

	cout << "Loading audio\n";
	audio = new AudioPlayer(100);
    Actors.Player()->Build();
    GameStarted.Fire([this](function<void(BaseFrame*)> subscriber) {
        subscriber(this);
    });
    
    // Subscribe to the pause window visibility events
    GameEventSubscriber::Subscribe<void(Control*,bool)>(&pauseWindow->VisibilityChanged, [this](Control *control, bool visible)
    {
        // If the pause window is visible, first person is not
        this->FirstPerson->Enable(!visible);
    });
    
    VoxEngine::SynchronousTask.RequestTask([this] ()
    {
        // Test getting a cubemap
        GLTextureCubeMap *cubemap = Textures.GetTexture<GLTextureCubeMap>("skybox/skybox");
        if(cubemap)
        {
            std::cout << "Cubemap is valid!! " << endl;
        }
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
    {
        pauseWindow->toggle();
    }
    
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
	vec2 viewPortSize = vec2((float)width,(float)height);
	//Save size in camera as well (for unprojection)
	Camera.UpdateViewSize(viewPortSize);
    Actors.Player()->WeaponCamera().UpdateViewSize(viewPortSize);
	//Update the texture caching system
	Textures.Refresh();

	float fogDistance = 0;
	//Apply view distance
	ViewDistance.CalculateAndApply(fogDistance,fpsCount.GetFps());


	//We add 1.5 to ground level. This assumes the person is 5ft between the ground
	//and his eye line
	vec3 pos = Actors.Player()->GetPosition();


	//Draw the frame
	//camera draw also sets up world light
	Camera.SetCameraView(pos,FirstPerson->GetLookVector());

	//Apply properties to each shader
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
	shaders3d->Lights.On();
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
	Particles.Draw(shaders);
	
	//Update the voxel debug counter
	Controls.Debug.Voxels = Voxels.GetLastVoxelCount();

	//Set up the 2D Shader
	GL2DProgram * shaders2d = SetWidthHeight(width, height);
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
	//Can only be used when the game is running
	BaseFrame* frame = (BaseFrame*)Frames::GetSystem(Frames::FRAME_GAME);
	_ASSERTE(typeid(*frame) == typeid(BaseFrame));
	return frame;
}

HUD* BaseFrame::GetHUD() {
	return &hud;
}
