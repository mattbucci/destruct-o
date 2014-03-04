
#pragma once

#include "stdafx.h"

#include "TileCell.h"

class GameTile {
	GameTile(int width, int height);
public:
	~GameTile();

	//Load a game tile from disk
	static GameTile * LoadTileFromDisk(string tileImagePath);
	//Load a game tile from memory
	static GameTile * LoadTileFromMemory(const vector<unsigned char> & tileData, unsigned int tileWidth, unsigned int tileHeight);

	//Recalculate stack heights for the given region of tile cells
	void CalculateStackSizes(unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy);

	//Save the tile to disk
	void SaveTile(string saveName);

	//Get the height of a position within this tile
	//don't call for positions outside of the tile
	TileCell * GetTile(vec2 pos);

	//Carves a square crater from fx,fy to tox,toy to depth "depth" and adds all removed voxels
	//to the removedVoxels value, the first three values are xyz and the forth value
	//is the material
	void Crater(int fx, int fy, int tox, int toy, int craterBottomZ, vector<vec4> & removedVoxels);

	//Loaded tile information is public for fastest access
	TileCell * Cells;
	int Width;
	int Height;
};