#include "TileHandler.h"

static const int tile_width = 256;
static const int tile_height = 256;

TileHandler::TileHandler(void)
{
}


TileHandler::~TileHandler(void)
{
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
		while(genQueue.size() <= 0) {
			//std::cout << "HANDLER THREAD WAIT\n";
			genCv.wait(gLck);
			//std::cout << "HANDLER THREAD WAKE\n";
		}

		//std::cout << "HANDLER THREAD START\n";

		//Assign Work Items
		while(genQueue.size() > 0) {
			//std::cout << "GRABBING WORK\n";
			//Grab Work from Queue
			vec2 pos = genQueue.front();
			GameTile * tile = GameTile::CreateGameTile(tile_width, tile_height, pos.x, pos.y);
			genQueue.pop_front();
			gLck.unlock();

			genRoutine(tile);
			//std::cout << "GENERATION COMPLETE\n";
			wLck.lock();
			//std::cout << "GRABBED WORLD LOCK\n";
			int s = worldSet.size();
			for(int i = 0; i < s; i++) {
				if(worldSet[i]->tile_x == pos.x && worldSet[i]->tile_y == pos.y) {
					delete worldSet[i];
					worldSet[i] = tile;
					//std::cout << "TILE X -> " << worldSet[i]->tile_x << " && TILE Y -> " << worldSet[i]->tile_y << "\n";
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

void TileHandler::genThread(GameTile * newTile) {
	_ASSERTE(false); // DO NOT CALL, PHASE OUT

	//Generate Terrain Data
	unsigned char* rawTile = generator->generateTile(newTile->tile_x, newTile->tile_y);

	//Assign Data to Container
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (newTile->Width * newTile->Height *  4));

	//Free Generatored Terrain Data
	delete rawTile;

	//Load Tile Data into GameTile
	GameTile::LoadTileFromMemoryIntoExisting(tile, newTile);

	//Obtain Lock on World Set
	std::unique_lock<std::mutex> wLck(worldMtx);
	_ASSERTE(wLck.owns_lock());

	//Push New Tile into World Set
	//worldSet[vec2(newTile->tile_x, newTile->tile_y)] = newTile;
	worldCv.notify_all();

	//Release Lock on World Set
	wLck.unlock();
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

	//std::cout << "MAIN - ATTEMPTING GRAB GEN LOCK\n";
	//Grab Lock
	genLck.lock();
	//std::cout << "MAIN - SUCCESSFUL GRAB GEN LOCK\n";

	//Add to Queue & Notify Handler
	genQueue.push_back(pos);
	//Add Empty Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreateGameTile(tile_width, tile_height, pos.x, pos.y));
	genCv.notify_one();

	//std::cout << "MAIN - PREDICTED TILE X -> " << pos.x << " && TILE Y -> " << pos.y << "\n";

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
	//Add Empty Tile to WorldSet for Duplicate Prevention
	worldSet.push_back(GameTile::CreateGameTile(tile_width, tile_height, pos.x, pos.y));
	genCv.notify_one();

	//Release Gen Queue Lock
	genLck.unlock();

}

GameTile * TileHandler::getTile(vec2 pos) {
	GameTile* tile = NULL;

	//Obtain Lock on World Set
	worldLck.lock();

	//Search World Set for Loaded Tile
	int s = worldSet.size();
	for(int i = 0; i < s; i++) {
		if(pos.x == worldSet[i]->tile_x && pos.y == worldSet[i]->tile_y) {
			//std::cout << "MATCHED EXISTING TILE - TILE X -> " << pos.x << " && TILE Y -> " << pos.y << std::endl;
			tile = worldSet[i];
			break;
		}
	}

	//If Tile Not Already in Worldset
	if(tile == NULL) {
		forceTile(pos);
		while(tile == NULL) {
			//std::cout << "MAIN THREAD WAIT\n";
			worldCv.wait(worldLck);
			//std::cout << "MAIN THREAD WAKE\n";
			//std::cout << "LOOKING FOR TILE X -> " << pos.x << " && TILE Y -> " << pos.y << std::endl;
			int s = worldSet.size();
			for(int i = 0; i < s; i++) {
				//std::cout << "\tTILE X -> " << worldSet[i]->tile_x << " && TILE Y -> " << worldSet[i]->tile_y << std::endl;
				if(pos.x == worldSet[i]->tile_x && pos.y == worldSet[i]->tile_y) {
					//std::cout << "TILE MATCH\n";
					tile = worldSet[i];
					break;
				}
			}
		}
	}

	for(int x_offset = -1; x_offset <= 1; x_offset++) {
		for(int y_offset = -1; y_offset <= 1; y_offset++) {
			predictTile(vec2(pos.x + x_offset, pos.y + y_offset));
		}
	}

	worldLck.unlock();

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