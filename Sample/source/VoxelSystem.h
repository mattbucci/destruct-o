#pragma once

#include "stdafx.h"
#include "ContiguousList.h"
#include "TileHandler.h"
#include "IntRect.h"
#include "TerrainChunkRenderer.h"

class GL3DProgram;
class GameTile;
class VoxelDrawSystem;
struct TileCell;
class TerrainGen;

class VoxelSystem {
	//A debug counter for the number of voxels rendered last frame
	int voxelCount;

	//The voxel texture
	GLuint textureId;

	//The Tile Handle
	TileHandler* tiles;

	VoxelDrawSystem * cellRenderer;
	TerrainChunkRenderer terrainRenderer;

	//Calls foreachFunction for each tile contained in the Rect "region"
	void forTilesInRect(IntRect region, function<void(GameTile * tile)> foreachFunction);
public:
	VoxelSystem();
	~VoxelSystem();

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
	void Draw(GL3DProgram * shader, vec3 drawPos, IntRect drawRegion);
	void Update(vec3 player_pos);
};