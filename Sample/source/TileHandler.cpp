#include "TileHandler.h"

#define TILE_CACHE_SIZE 16

TileHandler::TileHandler(void)
{
}


TileHandler::~TileHandler(void)
{
}

void TileHandler::init() {
	//Prevent Double Init
	_ASSERTE(handlerThread == NULL);

	//Create & Run Handler Thread
	handlerThread = new std::thread(handlerLoop);
	handlerThread->detach();
}

void TileHandler::handlerLoop() {
	//Lock for Shared Structures (Lock by Default)
	std::unique_lock<std::mutex> lck(mtx);

	//Ensure the Lock is Secured
	_ASSERTE(lck.owns_lock());

	//Begin Work Loop
	while(true) {
		while(genQueue.size() <= 0) {
			cv.wait(lck);
		}

		//Grab Lock
		lck.lock();

		//Assign Work Items
		while(genQueue.size() > 0) {

			//Grab Work from Queue
			GameTile * tile = genQueue.front();
			genQueue.pop();

			//Assign Work to Thread
			std::thread t(genThread, tile);
			t.detach();
		}

		//Release Lock
		lck.unlock();
	}

	//Tile Handler Should Never Exit
	_ASSERTE(false);
}

void TileHandler::genThread(GameTile * newTile) {
	//Generate Terrain Data
	unsigned char* rawTile = generator->generateTile(newTile->tile_x, newTile->tile_y);

	//Assign Data to Container
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (newTile->Width * newTile->Height *  4));

	//Free Generatored Terrain Data
	delete rawTile;

	//Load Tile Data into GameTile
	GameTile::LoadTileFromMemoryIntoExisting(tile, newTile);

	std::unique_lock<std::mutex> readyLck(readyMtx);
	_ASSERTE(readyLck.owns_lock());
	readyList.push_front(newTile);
	readyCv.notify_all();
	readyLck.unlock();
}

void TileHandler::predictTile(vec2 pos) {

	//TODO: REPLACE WITH VARIABLE TILE SIZE
	GameTile* predTile = GameTile::CreateGameTile(256, 256, pos.x, pos.y);

	//Grab Lock
	lck.lock();

	//Add to Queue & Notify Handler
	genQueue.push(predTile);
	cv.notify_one();

	//Release Lock
	lck.unlock();
}

GameTile * TileHandler::getTile(vec2 pos) {
	//return GameTile::CreateGameTile(256, 256, 0, 0);
	readyLck.lock();
	int s = readyList.size();
	for(int i = 0; i < s; i++) {
		if
	}
	readyLck.unlock();
}

TerrainGen* TileHandler::generator = new TerrainGen();

std::mutex TileHandler::mtx;
std::condition_variable TileHandler::cv;
std::unique_lock<std::mutex> TileHandler::lck(mtx, std::defer_lock);
std::queue<GameTile*> TileHandler::genQueue = std::queue<GameTile*>();

std::mutex TileHandler::readyMtx;
std::condition_variable TileHandler::readyCv;
std::unique_lock<std::mutex> TileHandler::readyLck(mtx, std::defer_lock);
std::list<GameTile*> TileHandler::readyList = std::list<GameTile*>();

std::thread* TileHandler::handlerThread = NULL;