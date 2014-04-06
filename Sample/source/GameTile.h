
#pragma once

#include "stdafx.h"

#include "TileCell.h"
#include "Structure.h"
#include "IntRect.h"

//The width/height of all tiles
//Must be divisible by 16
#define TILE_SIZE 256


class GameTile {
	GameTile();
	//Recalculate stack heights for the given region of tile cells
	static void CalculateStackSizes(TileCell * cells, unsigned int width, unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy);
	//Populates a large cell array
	//from a small cell array given the region in the small cell array that is given
	//there will always be one large cell for every 4 small cells
	static void DownsizeTile(TileCell * smallCells, TileCell * largeCells, unsigned int smallWidth, unsigned int & rx, unsigned int & ry, unsigned int & tox, unsigned int & toy);

	//Patch edge heights to be realistic values
	static void PatchStackEdges(TileCell * cellList, int cellWidth);
public:
	~GameTile();

	//Detail levels of the tile
	enum DetailLevel {
		DETAIL_HIGH,
		DETAIL_MEDIUM,
		DETAIL_LOW,
	};

	//Create Empty Tile (Allocated but Uninitialized Terrain Data)
	static GameTile * CreateGameTile(int x, int y);
	//Create Placeholder Tile (Tile_X & Tile_Y Initialized ONLY)
	static GameTile * CreatePlaceholderTile(int x, int y);
	//Load a game tile from disk
	static GameTile * LoadTileFromDisk(string tileImagePath);
	//Load a game tile from memory
	static GameTile * LoadTileFromMemory(const vector<unsigned char> & tileData);
	//Load a game tile from Memory (into existing GameTile)
	static void LoadTileFromMemoryIntoExisting(const vector<unsigned char> & tileData, GameTile * newTile);

	//Recalculate stack heights for the given region of tile cells
	void UpdateTileSection(unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy);

	//Save the tile to disk
	void SaveTile(string saveName);

	//Get the height of a position within this tile (relative to this tile)
	//don't call for positions outside of the tile
	TileCell * GetTileCell(vec2 pos);

	//Carves a square crater from fx,fy to tox,toy to depth "depth" and adds all removed voxels
	//to the removedVoxels value, the first three values are xyz and the forth value
	//is the material
	void Crater(int fx, int fy, int tox, int toy, int craterBottomZ, vector<vec4> & removedVoxels);

	//Render the given region using the specified detail level
	//the rect should be tile-relative coordinates
	void Render(GL3DProgram * shader, VoxelDrawSystem * drawSystem, IntRect rect, DetailLevel detail, int & voxelCount);

	//Loaded tile information is public for fastest access
	//An array of cells size 1 of size width*height
	TileCell * Cells;
	//An array of cells size 4 of size (width/4)*(height/4)
	TileCell * MediumCells;
	//An array of cells size 16 of size (width/16)*(height/16)
	TileCell * LargeCells;
	int tile_x;
	int tile_y;

	//Each tile can have Cities on it
	vector<vec3> Cities;

	//Each tile can have structures on it
	vector<Structure> Structures;
};