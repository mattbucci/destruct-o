#include "stdafx.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>
#include <vector>
#include "GameTile.h"
#include "TerrainGen.h"
#include "CityGen.h"
#include "Savable.h"
#include "IntegerGlmTypes.h"

#pragma once

//If a tile isn't used for this many seconds
//dump it to disk
#define TILE_LIFETIME 45

using namespace std;

class TileHandler : public Savable
{
	bool runThread;
	TerrainGen terraingenerator;
	CityGen citygenerator;

	mutex genRunningMtx;

	mutex genMtx;
	condition_variable genCv;
	list<vec2i> genQueue;

	mutex worldMtx;
	condition_variable worldCv;
	map<vec2c<vec2i>,GameTile*> worldSet;
	//Represents the positions of all tiles currently loaded
	//must have world lock to use
	ContiguousList<vec2i> cachedTiles;

	thread* handlerThread;

	void handlerLoop();
	void genThread(GameTile * newTile);
	//Construct a new tile, or load one from disk and construct it
	GameTile * genRoutine(vec2i pos);

	void forceTile(vec2i pos);
	void predictTile(vec2i pos);

	//A list of generated tiles which may or may not be in-memory
	//only access with the world lock
	ContiguousList<vec2i> listOfGeneratedTiles;

	//Retrieve a tile pointer if the tile is cached
	//data must still be checked to see if the tile is fully generated
	//you must acquire worldlck before calling this
	GameTile * findCachedTile(vec2i pos);

	//Retrieves a copy of compressed tile data for the given tile
	vector<unsigned char> RetrieveCompressedTileData(vec2i pos);

	//Retrieve the save directory for the current save
	string saveDirectory();

	//Retrieve the name of a tile file given its position
	string tileName(vec2i pos);

	//The seed of this world
	int worldSeed;
protected:
	//Package all tiles in save
	virtual void Save(Json::Value & parentValue);
	//Unpackage all tiles in load
	virtual void Load(Json::Value & parentValue, LoadData & loadData);
public:
	TileHandler();
	~TileHandler();

	void setSeed(int seed);
	int getSeed();

	GameTile * getTile(vec2i pos);

	//Use a radius around the player position to spawn new tiles
	//right now radius is 1 tile
	void Update(vec2i playerPos);

	//Save relevant information
	CLASS_DECLARATION(TileHandler)
		CLASS_MEMBER(worldSeed,ReflectionData::SAVE_INT32)
		CLASS_CONTAINER_MEMBER(cachedTiles,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2I)
		CLASS_CONTAINER_MEMBER(listOfGeneratedTiles,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2I);
	END_DECLARATION
};