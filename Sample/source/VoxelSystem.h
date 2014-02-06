#pragma once

#include "stdafx.h"

class GL3DProgram;
class GameTile;
class VoxelDrawSystem;

class VoxelSystem {
	//Tile data loaded
	GameTile * tileData;
	
	//A debug counter for the number of voxels rendered last frame
	int voxelCount;

	//The voxel texture
	GLuint textureId;


	VoxelDrawSystem * renderer;
public:
	VoxelSystem();
	~VoxelSystem();

	//Attempt to load a tile from disc
	bool LoadTile(string tileName);

	//Get the number of voxels drawn last frame
	int GetLastVoxelCount();

	//Draw the voxels in a region
	//atx,aty and tox, toy define two corners of the rectangle
	//of voxels which will be rendered
	void Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy);

	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};