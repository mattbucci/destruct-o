
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
	//pending
	static GameTile * LoadTileFromMemory(const vector<unsigned char> & tileData, unsigned int tileWidth, unsigned int tileHeight);

	//Save the tile to disk
	void SaveTile(string saveName);

	//Get the height of a position within this tile
	//don't call for positions outside of the tile
	float GetHeight(vec2 pos);

	//Loaded tile information is public for fastest access
	TileCell * Cells;
	int Width;
	int Height;
};