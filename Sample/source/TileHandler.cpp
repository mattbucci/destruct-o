#include "stdafx.h"
#include "TileHandler.h"
#include "OS.h"
#include "BaseFrame.h"
#include "lodepng.h"
#include "base64.h"
#include "Utilities.h"



TileHandler::~TileHandler(void)
{
	//Shut down the generation thread
	runThread = false;
	genCv.notify_all();
	handlerThread->join();
	delete handlerThread;
}

TileHandler::TileHandler() : 
	genLck(genMtx, std::defer_lock), 
	worldLck(worldMtx, std::defer_lock), 
	genRunningLck(genRunningMtx, defer_lock){
	runThread = true;
	handlerThread = NULL;
	//Initialize Terrain Generator
	terraingenerator.setTileSize(TILE_SIZE, TILE_SIZE);
	//static seed should be changed
	terraingenerator.setSeed(58902);

	//Create & Run Handler Thread
	handlerThread = new std::thread([this](){handlerLoop(); });
}

void TileHandler::handlerLoop() {
	//Lock for Generation List (Default to Locked)
	unique_lock<mutex> gLck(genMtx);
	unique_lock<mutex> wLck(worldMtx, std::defer_lock);

	//Ensure the Lock is Secured
	_ASSERTE(gLck.owns_lock());

	//Begin Work Loop
	while(runThread) {
		//Wait until work available
		while(genQueue.size() <= 0) {
			genCv.wait(gLck);
		}

		genRunningLck.lock();

		//Get Terrain Generation Work
		while(genQueue.size() > 0) {
			//Grab Work from Queue
			vec2 pos = genQueue.front();
			GameTile * tile = GameTile::CreateGameTile((int)pos.x, (int)pos.y);
			genQueue.pop_front();
			gLck.unlock();

			//Generation Routine
			genRoutine(tile);

			wLck.lock();
			//Add this tile to the list of tiles that exist in this world
			listOfGeneratedTiles.push_back(pos);
			//Add this tile to the list of tiles in memory right now
			cachedTiles.push_back(pos);
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

			//Unlock genRunningLock briefly
			//so if someone wants to stop the cycle they can
			genRunningLck.unlock();
			genRunningLck.lock();

			gLck.lock();
		}

		genRunningLck.unlock();
	}

	//Tile Handler Should Never Exit
	_ASSERTE(false);
}

void TileHandler::genRoutine(GameTile * newTile) {
	//Generate Terrain Data
	terraingenerator.generateTerrain(newTile);
	//Generate Cities
	citygenerator.GenerateCities(newTile);
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
	terraingenerator.setSeed(seed);
}

int TileHandler::getSeed() {
	return terraingenerator.getSeed();
}

GameTile * TileHandler::getTile(vec2 pos) {
	//Obtain Lock on World Set
	worldLck.lock();

	//Search World Set for Tile
	GameTile* tile = findCachedTile(pos);
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

//Retrieve a tile pointer safely if the tile is cached
GameTile * TileHandler::findCachedTile(vec2 pos) {
	//Ensure World Set Lock Owned
	_ASSERTE(worldLck.owns_lock());

	//Search World Set for Tile
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
			return worldSet[i];
		}
	}
	return NULL;
}

//Retrieve the save directory for the current save
string TileHandler::saveDirectory() {
	return Game()->GetSaveLocation() + "tiles/";
}

string TileHandler::tileName(vec2 pos) {
	return string("tile_") + Utilities::toString((int)pos.x) + "_" + Utilities::toString((int)pos.y) + ".tile";
}

//Retrieves a copy of compressed tile data for the given tile
vector<unsigned char> TileHandler::RetrieveCompressedTileData(vec2 pos) {
	//Ensure World Set Lock Owned
	_ASSERTE(worldLck.owns_lock());


	GameTile * tile = findCachedTile(pos);
	//ensure the internal array exists
	if (!tile->Cells)
		tile = NULL;

	if (tile == NULL) {
		//If the tile isn't cached it must have been offloaded to disk
		//lets read it off the disk
		//(borrow from lodepng)
		vector<unsigned char> tileData;
		lodepng::load_file(tileData,saveDirectory() + "tiles/" + tileName(pos));

		//If for some reason the file failed to load
		//tileData() will be empty and the function that calls this one
		//will deal with it
		return tileData;
	}
	else {
		//recompress the data
		return tile->SaveTileToMemory();
	}
}

//Package all tiles in save
void TileHandler::Save(Json::Value & parentValue) {
	//Stop generation during save/load
	//this is the only safe order to acquire these mutexes
	lock_guard<unique_lock<mutex>> lockerA(genRunningLck);
	lock_guard<unique_lock<mutex>> lockerB(worldLck);
	lock_guard<unique_lock<mutex>> lockerC(genLck);

	//Save tiles
	Json::Value & tiles = parentValue["tiles"];
	int tileNumber = 0;
	for (auto tilePosition : listOfGeneratedTiles) {
		//Get the tile data
		vector<unsigned char> tileData(RetrieveCompressedTileData(tilePosition));
		//Reserve space
		vector<unsigned char> base64Data(Base64encode_len(tileData.size()));
		//Encode into base 64
		Base64encode((char*)&base64Data.front(),(char*)&tileData.front(),tileData.size());
		//Save to json
		Json::Value & tile = tiles[tileNumber++];
		tile["x"] = tilePosition.x;
		tile["y"] = tilePosition.y;
		tile["base64data"] = string((char*)&base64Data.front(),base64Data.size());
	}

	//Continue with the save
	Savable::Save(parentValue);
}
//Unpackage all tiles in load
void TileHandler::Load(Json::Value & parentValue, LoadData & loadData) {
	//Stop generation during save/load
	//this is the only safe order to acquire these mutexes
	lock_guard<unique_lock<mutex>> lockerA(genRunningLck);
	lock_guard<unique_lock<mutex>> lockerB(worldLck);
	lock_guard<unique_lock<mutex>> lockerC(genLck);

	//Clean the state
	//genRunningMtx guarantees this is safe
	for (auto tile : worldSet)
		delete tile;
	worldSet.clear();
	genQueue.clear();

	//Load general data
	Savable::Load(parentValue,loadData);

	

	//Load tiles
	Json::Value & tiles = parentValue["tiles"];
	int tileNumber = 0;
	for (auto tile : tiles) {
		vec2 tilePosition((int)tile["x"].asFloat(),(int)tile["y"].asFloat());
		//Get the tile data
		string base64Data = tile["base64data"].asString();
		base64Data += '\0';
		//Reserve space
		vector<unsigned char> tileData(Base64decode_len(base64Data.c_str()));
		//Decode from base64
		Base64decode((char*)&tileData.front(),base64Data.c_str());
		//Now check, was this tile cached before?
		bool cached = false;
		for (auto pos : cachedTiles) {
			if (pos == tilePosition) {
				cached = true;
				break;
			}
		}

		//If it was cached, cache it again
		if (cached)
			worldSet.push_back(GameTile::LoadCompressedTileFromMemory(tileData));
		else {
			//Dump it to file
			//as it was before
			lodepng::save_file(tileData,saveDirectory() + "tiles/" + tileName(tilePosition));
		}
	}
}