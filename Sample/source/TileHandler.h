#include "stdafx.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>
#include <vector>
#include "GameTile.h"
#include "TerrainGen.h"

#pragma once
class TileHandler
{
	static TerrainGen* generator;

	static std::mutex genMtx;
	static std::condition_variable genCv;
	static std::unique_lock<std::mutex> genLck;
	static std::list<vec2> genQueue;

	static std::mutex worldMtx;
	static std::condition_variable worldCv;
	static std::unique_lock<std::mutex> worldLck;
	static std::vector<GameTile*> worldSet;

	static void addGenQueue(vec2 pos);
	static void addGenQueueFront(vec2 pos);

	static std::thread* handlerThread;
	static void handlerLoop();

	static void genThread(GameTile * newTile);
	static void genRoutine(GameTile * newTile);

	static void forceTile(vec2 pos);
	static void predictTile(vec2 pos);

	TileHandler(void);
	~TileHandler(void);
public:
	static void init();

	static void setSeed(int seed);
	static int getSeed();

	static GameTile * getTile(vec2 pos);
};