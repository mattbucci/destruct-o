#pragma once

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

class ActorPlayer;
class ParticleSystem;
class Demo;

class BaseFrame : public GameSystem {
	ActorPlayer * player;
	
	GLint uniformModelView;
	GLint uniformModelViewProjection;

	Demo * demo;
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

	//for notes on Build() see GameSystem::Build()
	void Build() override;

	//Build basic entities (such as the player)
	void OnFrameFocus() override;

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);



	//Draw happens whenever possible
	void Draw(double width, double height) override;
};