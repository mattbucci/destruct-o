
#pragma once

#include "stdafx.h"

class GL3DProgram;

//There are different voxel systems
//some chosen at compile time
//some chosen at run time
//all must implement these features
//to draw voxels
class VoxelDrawSystem {
public:
    virtual ~VoxelDrawSystem();
    
	//Called at the start of the draw cycle
	//does nothing in this render system
	virtual void startDraw(GL3DProgram * shader) = 0;

	//Add a voxel and maybe draw
	//Called per voxel
	virtual void pushVoxel(GL3DProgram * shader, vec3 pos,int materialId) = 0;

	//Called at the end of the draw cycle
	//draws any undrawn voxels
	virtual void finishDraw(GL3DProgram * shader) = 0;
};