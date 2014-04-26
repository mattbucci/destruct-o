#pragma once

#include "VoxEngine.h"
#include "ShaderGroup.h"

#include "GameSystem.h"

#include "VoxelSystem.h"
#include "GameCamera.h"
#include "ViewDistanceCalc.h"
#include "ActorSystem.h"
#include "FirstPersonModeMobile.h"
#include "FirstPersonMode.h"
#include "FirstPersonModeMobile.h"
#include "PhysicsSystem.h"
#include "ParticleCloud.h"
#include "Audio.h"
#include "DemoWindow.h"
#include "Notification.h"
#include "PauseWindow.h"
#include "PauseWindowMobile.h"
#include "Achievements.h"
#include "HUD.h"
#include "GLSkybox.h"

#include "Model.h"
#include "ModelInstance.h"
#include "ModelGroup.h"

#include "GameEvent.h"
#include "GameEventSubscriber.h"

class ActorPlayer;
class ParticleSystem;

//Retrieve base frame
BaseFrame * Game();

class BaseFrame : public GameSystem
{
	AudioPlayer * audio;

	DemoWindow * demoWindow;
	Notification notification;
	PauseWindow * pauseWindow;
    ModelGroup * models;
	HUD hud;
    GLSkybox   *skybox;
    
	//Setup all the global values for shaders
	//Sets up, acid shader and fog
	template <class T>
	void SetupShader(string shaderName, float fogDistance)
    {
		T * shader = (T*)shaders->GetShader(shaderName);
		shader->UseProgram();
		//Setup fog
		shader->Fog.SetFogColor(vec4(.65,.70,.76,1));
		shader->Fog.SetFogDistance(fogDistance);
		//Setup acid shader
		shader->Acid.SetCurrentTime(Game()->Now());
		//Acid factor currently managed by the demo system
		//this will be moved to a more powerful game logic system in the future
		shader->Acid.SetAcidFactor(0);
	}
protected:
	//Overload to tell the save system about handles created in the system
	virtual void Load(Json::Value & parentValue, LoadData & loadData);

public:
	BaseFrame(ShaderGroup * shaders);
	~BaseFrame();

	//Should be unique per-save
	string SaveName;

	//Get the save path to the particular save folder
	string GetSaveLocation();

	//This is the main game object
	//so it contains all the subsystems
	GameCamera Camera;
	VoxelSystem Voxels;
	ViewDistanceCalc ViewDistance;
	
	// First person mode will either be desktop or mobile variant, so use a pointer
	FirstPersonMode *FirstPerson;
	PhysicsSystem Physics;
	ActorSystem Actors;
	ParticleCloud Particles;

	//Private objects which must be initialized after everything else
	//goes here
private:
	Achievements achievements;
	//This is the reset save
	//it represents the initial state of the world when there isnt' a world
	//its loaded to force the state to clear when making a new world
	vector<unsigned char> resetSave;
public:

	//Randomly generate a new world
	//Should be called async because this takes a long time
    void NewWorld();

	//Build() now is manually called by voxengine
	void Build();

	//Build basic entities (such as the player)
	void OnFrameFocus() override;
	void OnFrameLeave() override;

	//Toggle Escape Menu
	void ToggleMenu();
	//Push Notification
	void PushNotification(string txt);

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(vector<InputEvent> inputEvents) override;

	//synchronously saves the game
	//returns true if successful
	bool Save(string saveFile);
	//synchronously loads the game over any existing data
	//returns true if successful
	bool Load(string saveFile);

	//Draw happens whenever possible
	void Draw(double width, double height) override;
    
    // Model group accessor
    ModelGroup* Models();

	// Get HUD
	HUD* GetHUD();
    
    // Function should return the desired glClear bits
    virtual GLenum ClearBits() override;

    GameEvent<void(BaseFrame*)> GameStarted;
    GameEvent<void(BaseFrame*)> GamePaused;
    GameEvent<void(BaseFrame*)> GameLoaded;
    
	//Define how the base frame saves
	CLASS_DECLARATION(BaseFrame)
		INHERITS_FROM(GameSystem)
		CLASS_MEMBER(FirstPerson,ReflectionData::SAVE_USEROWNEDHANDLE)
		CLASS_MEMBER(Physics,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(Actors,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(Voxels,ReflectionData::SAVE_INSTANCE)
	END_DECLARATION
};