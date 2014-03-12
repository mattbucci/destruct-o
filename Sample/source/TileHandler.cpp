#include "TileHandler.h"

static const int tile_width = 256;
static const int tile_height = 256;

TileHandler::TileHandler(void)
{
	_ASSERTE(false); //This Class should never be Instantiated
}


TileHandler::~TileHandler(void)
{
	_ASSERTE(false); //This Class should never be Instantiated
}

void TileHandler::init() {
	//Prevent Double Init
	_ASSERTE(handlerThread == NULL);

	//Initialize Terrain Generator
	generator = new TerrainGen();
	generator->setTileSize(tile_width, tile_height);
	srand(time(NULL));
	generator->setSeed(rand());

	//Create & Run Handler Thread
	handlerThread = new std::thread(handlerLoop);
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
			GameTile * tile = GameTile::CreateGameTile(tile_width, tile_height, pos.x, pos.y);
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
	unsigned char* rawTile = generator->generateTile(newTile->tile_x, newTile->tile_y);

	//Assign Data to Container
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (newTile->Width * newTile->Height *  4));

	//Free Generatored Terrain Data
	delete rawTile;

	//Load Tile Data into GameTile
	GameTile::LoadTileFromMemoryIntoExisting(tile, newTile);
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
	worldSet.push_back(GameTile::CreatePlaceholderTile(pos.x, pos.y));
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
	genQueue.push_back(pos);
	//Add Placeholder Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreatePlaceholderTile(pos.x, pos.y));
	genCv.notify_one();

	//Release Gen Queue Lock
	genLck.unlock();

}

void TileHandler::setSeed(int seed) {
	generator->setSeed(seed);
}

int TileHandler::getSeed() {
	return generator->getSeed();
}

GameTile * TileHandler::getTile(vec2 pos) {
	GameTile* tile = NULL;

	//Obtain Lock on World Set
	worldLck.lock();

	//Search World Set for Loaded Tile
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y && worldSet[i]->Cells != NULL) {
			tile = worldSet[i];
			break;
		}
	}

	//If Tile Not Already in Worldset
	if(tile == NULL) {
		//Force Tile to front of Generation Queue
		forceTile(pos);

		//While tile not Generated
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

	return tile;
}

TerrainGen* TileHandler::generator = NULL;

std::mutex TileHandler::genMtx;
std::condition_variable TileHandler::genCv;
std::unique_lock<std::mutex> TileHandler::genLck(genMtx, std::defer_lock);
std::list<vec2> TileHandler::genQueue = std::list<vec2>();

std::mutex TileHandler::worldMtx;
std::condition_variable TileHandler::worldCv;
std::unique_lock<std::mutex> TileHandler::worldLck(worldMtx, std::defer_lock);
std::vector<GameTile*> TileHandler::worldSet = std::vector<GameTile*>();

std::thread* TileHandler::handlerThread = NULL;