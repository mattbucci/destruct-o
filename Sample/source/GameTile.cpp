#include "stdafx.h"
#include "GameTile.h"
#include "lodepng.h"

GameTile::GameTile() {
	Cells = new TileCell[TILE_SIZE*TILE_SIZE];
}

GameTile::~GameTile() {
	if(Cells != NULL) 
		delete [] Cells;
}

GameTile * GameTile::CreateGameTile(int x, int y) {
	GameTile * newTile = new GameTile();
	newTile->tile_x = x;
	newTile->tile_y = y;
	return newTile;
}

GameTile * GameTile::CreatePlaceholderTile(int x, int y) {
	GameTile * newTile = new GameTile();
	newTile->tile_x = x;
	newTile->tile_y = y;
	return newTile;
}

//Load a game tile from disk
GameTile * GameTile::LoadTileFromDisk(string tileImagePath) {
	unsigned int width;
	unsigned int height;
	vector<unsigned char> tileData;

	cout << "Starting load of tile \"" << tileImagePath << "\"\n";
	//First load the tile map data from the image
	if (lodepng::decode(tileData,width,height,tileImagePath)) {
		cout << "\tImage load failed.\n";
		//Error
		return NULL;
	}
	_ASSERTE(width==TILE_SIZE);
	_ASSERTE(height==TILE_SIZE);
	cout << "\tLoaded image file, now converting to game tile data.\n";
	return LoadTileFromMemory(tileData);
}
//Load a game tile from memory
//pending
GameTile * GameTile::LoadTileFromMemory(const vector<unsigned char> & tileData) {
	_ASSERTE(tileData.size() == (TILE_SIZE*TILE_SIZE*4));
	cout << "Starting Load of tile from memory\n";
	GameTile * tile = new GameTile();
	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		//Load every RGBA pixel into a tile cell
		tile->Cells[i/4].height = tileData[i+0];
		tile->Cells[i/4].materialId = tileData[i+1];
		//Stack height must be calculated separately
		tile->Cells[i/4].stackHeight = 0;
	}

	cout << "\tLoaded game tile data. Now building stacks.\n";
	tile->UpdateTileSection(1,1,TILE_SIZE-1,TILE_SIZE-1);

	//Patch edge stack heights to be 2 (3 height) to cover most holes
	for (unsigned int i = 0; i < TILE_SIZE; i++) {
		tile->Cells[i].stackHeight = 2;
		tile->Cells[i+(TILE_SIZE-1)*TILE_SIZE].stackHeight = 2;
	}
	for (unsigned int i = 0; i < TILE_SIZE; i++) {
		tile->Cells[i*TILE_SIZE + 0].stackHeight = 2;
		tile->Cells[i*TILE_SIZE + TILE_SIZE - 1].stackHeight = 2;
	}

	cout << "\tTile Load Complete.\n";
	return tile;
}

void GameTile::LoadTileFromMemoryIntoExisting(const vector<unsigned char> & tileData, GameTile * newTile) {
	_ASSERTE(tileData.size() == (TILE_SIZE * TILE_SIZE * 4));

	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		newTile->Cells[i/4].height = tileData[i+0];
		newTile->Cells[i/4].materialId = tileData[i+1];
		newTile->Cells[i/4].stackHeight = 0;
	}

	newTile->UpdateTileSection(1,1,TILE_SIZE-1,TILE_SIZE-1);
	for (int i = 0; i < TILE_SIZE; i++) {
		newTile->Cells[i].stackHeight = 2;
		newTile->Cells[i+(TILE_SIZE-1)*TILE_SIZE].stackHeight = 2;
	}
	for (int i = 0; i < TILE_SIZE; i++) {
		newTile->Cells[i*TILE_SIZE + 0].stackHeight = 2;
		newTile->Cells[i*TILE_SIZE + TILE_SIZE - 1].stackHeight = 2;
	}
}

//Recalculate stack heights for the given region of tile cells
void GameTile::CalculateStackSizes(TileCell * cells, unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {
	for (unsigned int x = rx; x < tox; x++) {
		for (unsigned int y = ry; y < toy; y++) {
			unsigned char lowestHeight;
			unsigned char checkHeight;
			//Your stack size is the stack necessary to 
			//cover up the hole between you and your lowest neighboring voxel
			//so first find the lowest neighboring voxel
			lowestHeight = cells[y*(int)TILE_SIZE + (x + 1)].height;

			if ((checkHeight = cells[y*(int)TILE_SIZE + (x - 1)].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = cells[(y + 1)*(int)TILE_SIZE + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = cells[(y - 1)*(int)TILE_SIZE + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			//Now determine the stack height based off of the lowest height around you
			unsigned char myHeight = cells[y*(int)TILE_SIZE+x].height;
			if (lowestHeight < myHeight)
				cells[y*(int)TILE_SIZE+x].stackHeight = myHeight-lowestHeight;
		}
	}
}
//Populates a large cell array
//from a small cell array given the region in the small cell array that is given
void GameTile::DownsizeTile(TileCell * smallCells, TileCell * largeCells, unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {

}

//Recalculate stack heights for the given region of tile cells
void GameTile::UpdateTileSection(unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {

}

TileCell * GameTile::GetTileCell(vec2 pos) {
	_ASSERTE((pos.x >= 0) && (pos.y >= 0));
	_ASSERTE((pos.x < (float)TILE_SIZE) && (pos.y < (float)TILE_SIZE));
	//First lookup which tile the position is in
	int tileX = (int)floor(pos.x);
	int tileY = (int)floor(pos.y);

	return &Cells[tileY*TILE_SIZE+tileX];
}

//Carves a square crater from fx,fy to tox,toy to depth "depth" and adds all removed voxels
//to the removedVoxels value
void GameTile::Crater(int fx, int fy, int tox, int toy, int craterBottomZ, vector<vec4> & removedVoxels) {
	_ASSERTE((fx >= 0) && (fy >= 0));
	_ASSERTE((tox < (float)TILE_SIZE) && (toy < (float)TILE_SIZE));	
	_ASSERTE((fx <= tox) && (fy <= toy));
	_ASSERTE(craterBottomZ > 0);


	for (int x = fx; x <= tox; x++) {
		for (int y = fy; y <= toy; y++) {
			TileCell& cell = Cells[y*TILE_SIZE+x];
			int height = cell.height;
			int heightDiff = height-craterBottomZ;
			//Skip where the terrain does not intersect the crater
			if (heightDiff < 0)
				break;
			//Keep track of all removed voxels
			while (heightDiff >= 0) {
				removedVoxels.push_back(vec4(x+(tile_x*TILE_SIZE),y+(tile_y*TILE_SIZE),height,cell.materialId));
				heightDiff--;
				height--;
			}
			//Alter the height to create the crater
			Cells[y*TILE_SIZE+x].height = craterBottomZ-1;
		}
	}
	//Alter the values to be large enough for stack calculation
	fx--;
	fy--;
	tox+=2;
	toy+=2;
	//Limit the values to valid ranges
	if (fx < 1)
		fx = 1;
	if (fy < 1)
		fy = 1;
	if (tox > TILE_SIZE-1)
		tox = TILE_SIZE-1;
	if (toy > TILE_SIZE-1)
		toy = TILE_SIZE-1;
	//Rebuild the stack heights in this region
	UpdateTileSection(fx,fy,tox,toy);
}

//Save the tile to disk
void GameTile::SaveTile(string saveName) {
	//Convert the tile to RGBA pixel data
	vector<unsigned char> rawTileData(TILE_SIZE*TILE_SIZE*4);
	for (int y = 0; y < TILE_SIZE; y++) {
		for (int x = 0; x < TILE_SIZE; x++) {
			int cellIndex = (x+y*TILE_SIZE);
			rawTileData[cellIndex*4+0] = Cells[cellIndex].height;
			rawTileData[cellIndex*4+1] = Cells[cellIndex].materialId;
			rawTileData[cellIndex*4+2] = 0;
			rawTileData[cellIndex*4+3] = 0;
		}
	}
	unsigned int error = lodepng::encode(saveName.c_str(),rawTileData,TILE_SIZE,TILE_SIZE);
	if (error) {
		cout << "Failed to save tile. Lodepng error " << error << ": "<< lodepng_error_text(error) << "\n";
	}
}
