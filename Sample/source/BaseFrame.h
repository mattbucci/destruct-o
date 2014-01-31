#pragma once

#include "GameSystem.h"

#include "VoxelSystem.h"
#include "GameCamera.h"
class BaseFrame : public GameSystem {
	GameCamera camera;
	VoxelSystem voxels;
public:
	BaseFrame(ShaderGroup * shaders);
	~BaseFrame();

	//Draw happens whenever possible
	void Draw(double width, double height) override;
};