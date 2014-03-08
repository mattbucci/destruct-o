#include "stdafx.h"
#include "GameTile.h"
#include "lodepng.h"

GameTile::GameTile(int width, int height) {
	this->Width = width;
	this->Height = height;
	Cells = new TileCell[width*height];
	ready = false;
}
GameTile::~GameTile() {
	delete [] Cells;
}

GameTile * GameTile::CreateGameTile(int width, int height, int x, int y) {
	GameTile * newTile = new GameTile(width, height);
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
	cout << "\tLoaded image file, now converting to game tile data.\n";
	return LoadTileFromMemory(tileData,width,height);
}
//Load a game tile from memory
//pending
GameTile * GameTile::LoadTileFromMemory(const vector<unsigned char> & tileData, unsigned int width, unsigned int height) {
	_ASSERTE(tileData.size() == (width*height*4));
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
	tile->CalculateStackSizes(1,1,width-1,height-1);

	//Patch edge stack heights to be 2 (3 height) to cover most holes
	for (unsigned int i = 0; i < width; i++) {
		tile->Cells[i].stackHeight = 2;
		tile->Cells[i+(height-1)*width].stackHeight = 2;
	}
	for (unsigned int i = 0; i < height; i++) {
		tile->Cells[i*width + 0].stackHeight = 2;
		tile->Cells[i*width + height - 1].stackHeight = 2;
	}

	cout << "\tTile Load Complete.\n";
	return tile;
}

void GameTile::LoadTileFromMemoryIntoExisting(const vector<unsigned char> & tileData, GameTile * newTile) {
	_ASSERTE(tileData.size() == (newTile->Width * newTile->Height * 4));
	//cout << "Starting ASYNC Load of tile from memory\n";
	for (unsigned int i = 0; i < tileData.size(); i+= 4) {
		newTile->Cells[i/4].height = tileData[i+0];
		newTile->Cells[i/4].materialId = tileData[i+1];
		newTile->Cells[i/4].stackHeight = 0;
	}
	//cout << "\tLoaded game tile data. Now building stacks.\n";
	newTile->CalculateStackSizes(1,1,newTile->Width-1,newTile->Height-1);
	for (unsigned int i = 0; i < newTile->Width; i++) {
		newTile->Cells[i].stackHeight = 2;
		newTile->Cells[i+(newTile->Height-1)*newTile->Width].stackHeight = 2;
	}
	for (unsigned int i = 0; i < newTile->Height; i++) {
		newTile->Cells[i*newTile->Width + 0].stackHeight = 2;
		newTile->Cells[i*newTile->Width + newTile->Height - 1].stackHeight = 2;
	}
	//cout << "\tTile Load Complete.\n";
}

void GameTile::CalculateStackSizes(unsigned int rx, unsigned int ry, unsigned int tox, unsigned int toy) {
	for (unsigned int x = rx; x < tox; x++) {
		for (unsigned int y = ry; y < toy; y++) {
			unsigned char lowestHeight;
			unsigned char checkHeight;
			//Your stack size is the stack necessary to 
			//cover up the hole between you and your lowest neighboring voxel
			//so first find the lowest neighboring voxel
			lowestHeight = Cells[y*(int)Width + (x + 1)].height;

			if ((checkHeight = Cells[y*(int)Width + (x - 1)].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = Cells[(y + 1)*(int)Width + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			if ((checkHeight = Cells[(y - 1)*(int)Width + x].height) < lowestHeight)
				lowestHeight = checkHeight;
			//Now determine the stack height based off of the lowest height around you
			unsigned char myHeight = Cells[y*(int)Width+x].height;
			if (lowestHeight < myHeight)
				Cells[y*(int)Width+x].stackHeight = myHeight-lowestHeight;
		}
	}
}

TileCell * GameTile::GetTileCell(vec2 pos) {
	_ASSERTE((pos.x >= 0) && (pos.y >= 0));
	_ASSERTE((pos.x < (float)Width) && (pos.y < (float)Height));
	//First lookup which tile the position is in
	int tileX = (int)floor(pos.x);
	int tileY = (int)floor(pos.y);

	return &Cells[tileY*Width+tileX];
}

//Carves a square crater from fx,fy to tox,toy to depth "depth" and adds all removed voxels
//to the removedVoxels value
void GameTile::Crater(int fx, int fy, int tox, int toy, int craterBottomZ, vector<vec4> & removedVoxels) {
	_ASSERTE((fx >= 0) && (fy >= 0));
	_ASSERTE((tox < (float)Width) && (toy < (float)Height));	
	_ASSERTE((fx <= tox) && (fy <= toy));
	_ASSERTE(craterBottomZ > 0);


	for (int x = fx; x <= tox; x++) {
		for (int y = fy; y <= toy; y++) {
			TileCell& cell = Cells[y*Width+x];
			int height = cell.height;
			int heightDiff = height-craterBottomZ;
			//Skip where the terrain does not intersect the crater
			if (heightDiff < 0)
				break;
			//Keep track of all removed voxels
			while (heightDiff >= 0) {
				removedVoxels.push_back(vec4(x+(tile_x*256),y+(tile_y*256),height,cell.materialId));
				heightDiff--;
				height--;
			}
			//Alter the height to create the crater
			Cells[y*Width+x].height = craterBottomZ-1;
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
	if (tox > Width-1)
		tox = Width-1;
	if (toy > Height-1)
		toy = Height-1;
	//Rebuild the stack heights in this region
	CalculateStackSizes(fx,fy,tox,toy);
}

//Save the tile to disk
void GameTile::SaveTile(string saveName) {
	//Convert the tile to RGBA pixel data
	vector<unsigned char> rawTileData(Width*Height*4);
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			int cellIndex = (x+y*Height);
			rawTileData[cellIndex*4+0] = Cells[cellIndex].height;
			rawTileData[cellIndex*4+1] = Cells[cellIndex].materialId;
			rawTileData[cellIndex*4+2] = 0;
			rawTileData[cellIndex*4+3] = 0;
		}
	}
	unsigned int error = lodepng::encode(saveName.c_str(),rawTileData,Width,Height);
	if (error) {
		cout << "Failed to save tile. Lodepng error " << error << ": "<< lodepng_error_text(error) << "\n";
	}
}
