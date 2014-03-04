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
	GameTile * tileData;
	ContiguousList<GameTile> world;
	TerrainGen * generator;
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
	bool GenWorld(int seed);

	//Generate a Tile
	bool LoadTile(int seed, int x, int y);

	bool GenTile(int x, int y);

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
	void Update(vec2 player_pos);
	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};