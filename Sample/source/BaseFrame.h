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

#include "Model.h"
#include "ModelInstance.h"

class ActorPlayer;
class ParticleSystem;
class Demo;

class BaseFrame : public GameSystem, public Savable {
	ActorPlayer * player;
	AudioPlayer * audio;

	Demo * demo;
	DemoWindow * demoWindow;
	Notification * notification;
	PauseWindow * pauseWindow;
    Achievements * achievements;

	//Setup all the global values for shaders
	//Sets up, acid shader and fog
	template <class T>
	void SetupShader(string shaderName, float fogDistance) {
		T * shader = (T*)shaders->GetShader(shaderName);
		shader->UseProgram();
		//Setup fog
		shader->Fog.SetFogColor(vec4(.5,.5,.5,1));
		shader->Fog.SetFogDistance(fogDistance);
		//Setup acid shader
		shader->Acid.SetCurrentTime(VoxEngine::GetGameSimTime());
		//Acid factor currently managed by the demo system
		//this will be moved to a more powerful game logic system in the future
		shader->Acid.SetAcidFactor(demo->CurrentAcidStrength);
	}
protected:
	//Overload to tell the save system about handles created in the system
	virtual void Load(Json::Value & parentValue, LoadData & loadData);

public:
	BaseFrame(ShaderGroup * shaders);
	~BaseFrame();

	//This is the main game object
	//so it contains all the subsystems
	GameCamera Camera;
	VoxelSystem Voxels;
	ViewDistanceCalc ViewDistance;
	ActorSystem Actors;
	
	// First person mode will either be desktop or mobile variant, so use a pointer
	FirstPersonMode *FirstPerson;
	PhysicsSystem Physics;
	ParticleCloud Particles;
    
    Model *model[4];
    std::vector<ModelInstance *> modelInstances;

	//for notes on Build() see GameSystem::Build()
	void Build() override;

	//Build basic entities (such as the player)
	void OnFrameFocus() override;

	//Toggle Escape Menu
	void ToggleMenu();
	//Push Notification
	void PushNotification(string txt);

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);

	//synchronously saves the game
	//returns true if successful
	bool Save(string saveFile);
	//synchronously loads the game over any existing data
	//returns true if successful
	bool Load(string saveFile);

	//Draw happens whenever possible
	void Draw(double width, double height) override;

	//Define how the base frame saves
	CLASS_DECLARATION(BaseFrame)
		CLASS_MEMBER(FirstPerson,ReflectionData::SAVE_USEROWNEDHANDLE)
		CLASS_MEMBER(Physics,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(Actors,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(player,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};