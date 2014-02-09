#pragma once

#include "GameSystem.h"

#include "VoxelSystem.h"
#include "GameCamera.h"
#include "ViewDistanceCalc.h"
#include "ActorSystem.h"
#include "FirstPersonMode.h"

class BaseFrame : public GameSystem {

	
	GLint uniformModelView;
	GLint uniformModelViewProjection;
public:
	BaseFrame(ShaderGroup * shaders);
	~BaseFrame();

	//This is the main game object
	//so it contains all the subsystems
	GameCamera Camera;
	VoxelSystem Voxels;
	ViewDistanceCalc ViewDistance;
	ActorSystem Actors;
	FirstPersonMode FirstPerson;

	//for notes on Build() see GameSystem::Build()
	void Build() override;

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);



	//Draw happens whenever possible
	void Draw(double width, double height) override;
};