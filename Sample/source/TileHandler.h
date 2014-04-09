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

#pragma once

using namespace std;

class TileHandler
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
};