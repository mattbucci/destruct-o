#pragma once

#include "stdafx.h"
#include "ContiguousList.h"
class GL3DProgram;
class GameTile;
class VoxelDrawSystem;
class TileCell;
class TerrainGen;

class VoxelSystem {
	//Tile data loaded
	ContiguousList<GameTile> world;
	TerrainGen * generator;
	//A debug counter for the number of voxels rendered last frame
	int voxelCount;

	//The voxel texture
	GLuint textureId;

	VoxelDrawSystem * renderer;

public:
	VoxelSystem();
	~VoxelSystem();

	//Attempt to load a world from disc
	bool LoadWorld(string saveName);

	//Attempt to Generate a new world
	bool GenWorld(int seed);

	//Load a tile, NULL if fail
	GameTile * LoadTile(vec2 pos);

	//Generate a Tile, NULL if fail, should never be fail so assert
	GameTile * GenTile(vec2 pos);

	//Get a Tile
	GameTile * GetTile(vec2 pos);

	//Get the number of voxels drawn last frame
	int GetLastVoxelCount();

	//Get a pointer to the exact tile cell of a position anywhere in the world
	TileCell * GetTileCellAt(vec2 pos);

	//Get the height of a specific position
	//all positions acceptable
	float GetPositionHeight(vec2 pos);

	//Get the stack size of a specific position
	//all positions acceptable
	int GetPositionStackSize(vec2 pos);

	//Pain a new material at position onto any voxel in that tile
	void Paint(vec2 pos, int newMaterial);

	//Deforms a region of voxels, punching a crater into the given position
	//all voxels removed are returned as positions
	vector<vec4> Crater(vec3 pos, float size);

	//Draw the voxels in a region
	//atx,aty and tox, toy define two corners of the rectangle
	//of voxels which will be rendered
	void Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy);
	void Update(vec3 player_pos);
	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};