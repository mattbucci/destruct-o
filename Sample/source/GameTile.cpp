#include "stdafx.h"
#include "GameTile.h"
#include "lodepng.h"

GameTile::GameTile(int width, int height) {
	this->Width = width;
	this->Height = height;
	Cells = new TileCell[width*height];
}
GameTile::~GameTile() {
	delete [] Cells;
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
	cout << "\tLoaded image file, now converting to game tile data.\n";
	return LoadTileFromMemory(tileData,width,height);
}
//Load a game tile from memory
//pending
GameTile * GameTile::LoadTileFromMemory(const vector<unsigned char> & tileData, unsigned int width, unsigned int height) {
	cout << "Starting Load of tile from memory\n";
	GameTile * tile = new GameTile((int)width,(int)height);
	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		//Load every RGBA pixel into a tile cell
		tile->Cells[i/4].height = tileData[i+0];
		tile->Cells[i/4].materialId = tileData[i+1];
		//Stack height must be calculated separately
		tile->Cells[i/4].stackHeight = 0;
	}

	cout << "\tLoaded game tile data. Now building stacks.\n";
	for (unsigned int x = 1; x < width-1; x++) {
		for (unsigned int y = 1; y < height-1; y++) {
			unsigned char lowestHeight;
			unsigned char checkHeight;
			//Your stack size is the stack necessary to 
			//cover up the hole between you and your lowest neighboring voxel
			//so first find the lowest neighboring voxel
			lowestHeight = tile->Cells[y*(int)width+(x+1)].height;

			if ((checkHeight = tile->Cells[y*(int)width+(x-1)].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = tile->Cells[(y+1)*(int)width+x].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = tile->Cells[(y-1)*(int)width+x].height) < lowestHeight)
				lowestHeight = checkHeight;
			//Now determine the stack height based off of the lowest height around you
			unsigned char myHeight = tile->Cells[y*(int)width+x].height;
			if (lowestHeight < myHeight)
				tile->Cells[y*(int)width+x].stackHeight = myHeight-lowestHeight;
		}
	}
	cout << "\tTile Load Complete.\n";
	return tile;
}

float GameTile::GetHeight(vec2 pos) {
	_ASSERTE((pos.x >= 0) && (pos.y >= 0));
	_ASSERTE((pos.x < (float)Width) && (pos.y < (float)Height));
	//First lookup which tile the position is in
	int tileX = (int)floor(pos.x);
	int tileY = (int)floor(pos.y);
	//Now get the height of that tile
	return Cells[tileY*Width+tileX].height;
}

//Save the tile to disk
void GameTile::SaveTile(string saveName) {
	//stub
}
