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

#pragma once

using namespace std;

class TileHandler : public Savable
{
	TerrainGen* terraingenerator;
	CityGen* citygenerator;

	mutex genMtx;
	condition_variable genCv;
	unique_lock<mutex> genLck;
	list<vec2> genQueue;

	mutex worldMtx;
	condition_variable worldCv;
	unique_lock<mutex> worldLck;
	vector<GameTile*> worldSet;

	thread* handlerThread;

	void handlerLoop();
	void genThread(GameTile * newTile);
	void genRoutine(GameTile * newTile);

	void forceTile(vec2 pos);
	void predictTile(vec2 pos);

	//A list of generated tiles which may or may not be in-memory
	vector<vec2> listOfGeneratedTiles;
public:
	TileHandler() : genLck(genMtx, std::defer_lock), worldLck(worldMtx, std::defer_lock){
		terraingenerator = NULL;
		handlerThread = NULL;
	};
	~TileHandler();
	void init();

	void setSeed(int seed);
	int getSeed();

	GameTile * getTile(vec2 pos);

	//Save relevant information
	CLASS_DECLARATION(TileHandler)
		//CLASS_MEMBER(allVoxels,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_CONTAINER_MEMBER(listOfGeneratedTiles,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_VEC2);
	END_DECLARATION
};