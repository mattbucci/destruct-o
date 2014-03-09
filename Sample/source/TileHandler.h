#include "stdafx.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <queue>
#include "GameTile.h"
#include "TerrainGen.h"

#pragma once
class TileHandler
{
	static TerrainGen* generator;

	static std::mutex mtx;
	static std::condition_variable cv;
	static std::unique_lock<std::mutex> lck;
	static std::queue<GameTile*> genQueue;

	static std::mutex readyMtx;
	static std::condition_variable readyCv;
	static std::unique_lock<std::mutex> readyLck;
	static std::list<GameTile*> readyTiles;

	static std::thread* handlerThread;

	static void handlerLoop();
	static void genThread(GameTile * tile);

	TileHandler(void);
	~TileHandler(void);
public:
	static void init();
	static void predictTile(vec2 pos);
	static GameTile * getTile(vec2 pos);
};