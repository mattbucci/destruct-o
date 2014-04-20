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

TileHandler::TileHandler()  {
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
	unique_lock<mutex> genLck(genMtx,defer_lock);
	unique_lock<mutex> worldLck(worldMtx,defer_lock);
	unique_lock<mutex> genRunningLck(genRunningMtx,defer_lock);


	//Gen lock locked by default
	genLck.lock();


	//Ensure the Lock is Secured
	_ASSERTE(genLck.owns_lock());

	//Begin Work Loop
	while(runThread) {
		//Wait until work available
		while(genQueue.size() <= 0) {
			genCv.wait(genLck);
		}

		genRunningLck.lock();

		//Get Terrain Generation Work
		while(genQueue.size() > 0) {
			//Grab Work from Queue
			vec2 pos = genQueue.front();
			genQueue.pop_front();
			genLck.unlock();

			//Generation Routine
			GameTile * tile = genRoutine(pos);

			worldLck.lock();
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
			worldLck.unlock();
			worldCv.notify_all();

			//Unlock genRunningLock briefly
			//so if someone wants to stop the cycle they can
			genRunningLck.unlock();
			genRunningLck.lock();

			genLck.lock();
		}

		//Check if any tiles have expired
		//Note: it is only safe to acquire locks in the order: world,gen
		//so we must unlock and relock to preserve that order
		genLck.unlock();
		

		//Now check the worldset
		worldLck.lock();
		double now = Game()->Now();
		for (auto it = worldSet.begin(); it != worldSet.end();) {
			if (now >= (*it)->UseByDate) {
				vec2 tilePos = vec2((*it)->tile_x,(*it)->tile_y);
				//Cache to disk
				(*it)->SaveTile(saveDirectory() + tileName(tilePos));

				//Erase from the list of tiles in memory
				cachedTiles.erase(tilePos);
				//Cleanup tile
				delete *it;
				it = worldSet.erase(it);
			}
			else
				 it++;
		}

		worldLck.unlock();

		genLck.lock();
		genRunningLck.unlock();
	}
	//Unlock any remaining locks
	genLck.unlock();
}

//Construct a new tile, or load one from disk and construct it
GameTile * TileHandler::genRoutine(vec2 pos) {
 	bool tileOnDisk = false;
	
	{
		//Lock the world mutex
		lock_guard<mutex> locker(worldMtx);
		//Check if the tile is in the list of generated tiles
		for (auto genPos : listOfGeneratedTiles)
			if (pos == genPos) {
				tileOnDisk = true;
				break;
			}
	}


	
	if (tileOnDisk) {
		//If the tile is on disk load it off
		GameTile * gameTile = GameTile::LoadTileFromDisk(saveDirectory() + tileName(pos));
		if (gameTile != NULL) {
			gameTile->tile_x = (int)pos.x;
			gameTile->tile_y = (int)pos.y;
			return gameTile;
		}
		cout << "Error! Data lost. Previously generated tile is being overwritten by newly generated tile because tile was missing from disk.\n";
	}
	//Check if the file is in the list of created files
	GameTile * newTile = GameTile::CreateGameTile((int)pos.x, (int)pos.y);
	//Generate Terrain Data
	terraingenerator.generateTerrain(newTile);
	//Generate Cities
	citygenerator.GenerateCities(newTile);
	return newTile;
}

void TileHandler::predictTile(vec2 pos) {
	// Check if Tile Already Exists
	bool exists = false;
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
			//Still in use maybe	
			worldSet[i]->UseByDate = Game()->Now()+TILE_LIFETIME;
			exists = true;
			break;
		}
	}
	if(exists) {
		// If Exists, Don't Add to Queue
		return;
	}

	//Grab Lock
	lock_guard<mutex> locked(genMtx);

	//Add to Queue & Notify Handler
	genQueue.push_back(pos);
	//Add Placeholder Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreatePlaceholderTile((int)pos.x, (int)pos.y));
	genCv.notify_one();
}

void TileHandler::forceTile(vec2 pos) {
		//Grab gen Lock
	lock_guard<mutex> locked(genMtx);

	//Add to Queue & Notify Handler
	genQueue.push_front(pos);
	//Add Placeholder Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreatePlaceholderTile((int)pos.x, (int)pos.y));
	genCv.notify_one();
}

void TileHandler::setSeed(int seed) {
	terraingenerator.setSeed(seed);
}

int TileHandler::getSeed() {
	return terraingenerator.getSeed();
}

GameTile * TileHandler::getTile(vec2 pos) {
	//Obtain Lock on World Set
	unique_lock<mutex> worldLck(worldMtx,defer_lock);
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
		cout << "WARNING: SYNCRONOUS TILE GENERATION" << endl;

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

	tile->UseByDate = Game()->Now()+TILE_LIFETIME;

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

	GameTile * tile = findCachedTile(pos);
	//ensure the internal array exists
	if ((tile != NULL) && (!tile->Cells))
		tile = NULL;

	if (tile == NULL) {
		//If the tile isn't cached it must have been offloaded to disk
		//lets read it off the disk
		//(borrow from lodepng)
		vector<unsigned char> tileData;
		lodepng::load_file(tileData,saveDirectory() + tileName(pos));

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
	lock_guard<mutex> lockerA(genRunningMtx);
	lock_guard<mutex> lockerB(worldMtx);
	lock_guard<mutex> lockerC(genMtx);

	//Create relevant directories
	OS::BuildPath(saveDirectory());

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
	lock_guard<mutex> lockerA(genRunningMtx);
	lock_guard<mutex> lockerB(worldMtx);
	lock_guard<mutex> lockerC(genMtx);

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
		if (cached) {
			GameTile * tile = GameTile::LoadCompressedTileFromMemory(tileData);
			tile->tile_x = (int)tilePosition.x;
			tile->tile_y = (int)tilePosition.y;
			worldSet.push_back(tile);
		}
		else {
			//Dump it to file
			//as it was before
			lodepng::save_file(tileData,saveDirectory() + tileName(tilePosition));
		}
	}
}
