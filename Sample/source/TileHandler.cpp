#include "stdafx.h"
#include "TileHandler.h"

const int tile_width = 512;
const int tile_height = 512;


TileHandler::~TileHandler(void)
{
}

void TileHandler::init() {
	//Initialize Terrain Generator
	terraingenerator = new TerrainGen();
	terraingenerator->setTileSize(tile_width, tile_height);
	//srand((unsigned int)time(NULL));
    srand(69);
	terraingenerator->setSeed(rand());

	//Initalize City Generator
	citygenerator = new CityGen();

	//Create & Run Handler Thread
	handlerThread = new std::thread([this](){handlerLoop(); });
	handlerThread->detach();
}

void TileHandler::handlerLoop() {
	//Lock for Generation List (Default to Locked)
	std::unique_lock<std::mutex> gLck(genMtx);
	std::unique_lock<std::mutex> wLck(worldMtx, std::defer_lock);

	//Ensure the Lock is Secured
	_ASSERTE(gLck.owns_lock());

	//Begin Work Loop
	while(true) {
		//Wait until work available
		while(genQueue.size() <= 0) {
			genCv.wait(gLck);
		}

		//Get Terrain Generation Work
		while(genQueue.size() > 0) {
			//Grab Work from Queue
			vec2 pos = genQueue.front();
			GameTile * tile = GameTile::CreateGameTile(tile_width, tile_height, (int)pos.x, (int)pos.y);
			genQueue.pop_front();
			gLck.unlock();

			//Generation Routine
			genRoutine(tile);

			wLck.lock();
			int s = worldSet.size();
			for(int i = 0; i < s; i++) {
				if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
					delete worldSet[i];
					worldSet[i] = tile;
					break;
				}
			}
			worldCv.notify_all();
			wLck.unlock();

			gLck.lock();
		}
	}

	//Tile Handler Should Never Exit
	_ASSERTE(false);
}

void TileHandler::genRoutine(GameTile * newTile) {
	//Generate Terrain Data
	terraingenerator->generateTerrain(newTile);
	//Generate Cities
	citygenerator->GenerateCities(newTile);
}

void TileHandler::predictTile(vec2 pos) {

	//Ensure World Set Lock Owned
	_ASSERTE(worldLck.owns_lock());

	// Check if Tile Already Exists
	bool exists = false;
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
			exists = true;
			break;
		}
	}
	if(exists) {
		// If Exists, Don't Add to Queue
		return;
	}

	//Grab Lock
	genLck.lock();

	//Add to Queue & Notify Handler
	genQueue.push_back(pos);
	//Add Placeholder Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreatePlaceholderTile((int)pos.x, (int)pos.y));
	genCv.notify_one();

	//Release Lock
	genLck.unlock();
}

void TileHandler::forceTile(vec2 pos) {

	//Ensure World Set Lock Owned
	_ASSERTE(worldLck.owns_lock());

	//Grab Gen Queue Lock
	genLck.lock();

	//Add to Queue & Notify Handler
	genQueue.push_front(pos);
	//Add Placeholder Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreatePlaceholderTile((int)pos.x, (int)pos.y));
	genCv.notify_one();

	//Release Gen Queue Lock
	genLck.unlock();

}

void TileHandler::setSeed(int seed) {
	terraingenerator->setSeed(seed);
}

int TileHandler::getSeed() {
	return terraingenerator->getSeed();
}

GameTile * TileHandler::getTile(vec2 pos) {
	//Obtain Lock on World Set
	worldLck.lock();

	//Tile Pointer
	GameTile* tile = NULL;

	//Search World Set for Tile
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
			tile = worldSet[i];
			break;
		}
	}

	bool wait = true;

	//If Tile Not Already Generating
	if(tile == NULL) {
		//Force Tile to front of Generation Queue
		forceTile(pos);
	} else {
		//If Tile Exists, but Data not There
		if(tile->Cells != NULL) {
			//Tile Exists & Ready
			//No Waiting Necessary
			wait = false;
		} else {
			//Tile Being Generated
		}
	}

	if(wait) {
		tile = NULL;
		//While tile not Ready
		while(tile == NULL) {
			//Wait for World Update
			worldCv.wait(worldLck);

			//Check for Expected Tile
			int s = worldSet.size();
			for(int i = 0; i < s; i++) {
				if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y && worldSet[i]->Cells != NULL) {
					//On Match, Grab Reference
					tile = worldSet[i];
					break;
				}
			}
		}
	}

	//Predict Neighboring Tiles
	for(int x_offset = -1; x_offset <= 1; x_offset++) {
		for(int y_offset = -1; y_offset <= 1; y_offset++) {
			predictTile(vec2(pos.x + x_offset, pos.y + y_offset));
		}
	}

	//Release World Set Lock
	worldLck.unlock();

	//Guarantee a Tile is Returned
	_ASSERTE(tile != NULL);
	//Guarantee Tile Returned is Generated
	_ASSERTE(tile->Cells != NULL);

	return tile;
}
