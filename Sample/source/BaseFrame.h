#pragma once

#include "GameSystem.h"

#include "VoxelSystem.h"
#include "GameCamera.h"
#include "ViewDistanceCalc.h"
class BaseFrame : public GameSystem {
	GameCamera camera;
	VoxelSystem voxels;
	ViewDistanceCalc viewDistance;
public:
	BaseFrame(ShaderGroup * shaders);
	~BaseFrame();

	//Override update to remove FPS control
	//provided by the game system
	virtual bool Update(double delta,double now, vector<InputEvent> inputEvents);


	//Draw happens whenever possible
	void Draw(double width, double height) override;
};