#pragma once

#include "stdafx.h"
#include "ContiguousList.h"
class GL3DProgram;
class GameTile;
class VoxelDrawSystem;

class VoxelSystem {
	//Tile data loaded
	GameTile * tileData;
	ContiguousList<GameTile> world;
	//A debug counter for the number of voxels rendered last frame
	int voxelCount;

	//The voxel texture
	GLuint textureId;


	VoxelDrawSystem * renderer;

	struct tile_renderinfo {
		float x_start;
		float x_end;
		float y_start;
		float y_end;
		int   tile_x;
		int   tile_y; 
	};
public:
	VoxelSystem();
	~VoxelSystem();

	//Attempt to load a world from disc
	bool LoadWorld(string saveName);

	//Attempt to Generate a new world
	bool GenWorld();

	//Get the number of voxels drawn last frame
	int GetLastVoxelCount();

	//Get the height of a specific position
	//all positions acceptable
	float GetPositionHeight(vec2 pos);

	//Draw the voxels in a region
	//atx,aty and tox, toy define two corners of the rectangle
	//of voxels which will be rendered
	void Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy);

	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};