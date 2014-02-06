
#pragma once

#include "stdafx.h"

class GameTile {
	GameTile();
public:
	~GameTile();

	//Load a game tile from disk
	static GameTile * LoadTileFromDisk(string tileImagePath);
	//Load a game tile from memory
	//pending
	//static GameTile * LoadTileFromMemory(const vector<unsigned char> & tileData, unsigned int tileWidth, unsigned int tileHeight);

	void SaveTile();
};