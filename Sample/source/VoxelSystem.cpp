#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"
#include "TerrainGen.h"

#include "GL3DProgram.h"
#include "OS.h"

#include "GameTile.h"

#include "VoxelDrawSystem.h"
#include "InstancedVoxelRenderSystem.h"
#include "BasicVoxelRenderSystem.h"
#include <sstream>
#include <thread>

static const int tile_width = 256;
static const int tile_height = 256;

TerrainGen * VoxelSystem::generator = new TerrainGen();

VoxelSystem::VoxelSystem() {
	//TODO: Remove This
	//Init Terrain Generator
	//generator = new TerrainGen();
	//generator->setTileSize(tile_width, tile_height);
	//Set up random function
	//srand(time(NULL));

	renderer = VoxelDrawSystem::BuildAppropriateSystem();

	//Load the tile textures
	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;

	//A smart system would have multiple res tiles and would automatically select
	//one appropriate for the system its running on
	lodepng::decode(image, textureWidth, textureHeight, "terrain/tiles-lowres.png");

	//I should have moved the png->texture into a utility library
	//later...
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth,
		textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		&image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
}
VoxelSystem::~VoxelSystem() {
	delete renderer;
}

bool VoxelSystem::LoadWorld(string saveName) {
	//Attempt to load a tile from disc
	//This happens syncronously before the player loads, Generate just what we need and setup the terrain generator
	if (0) {
		//hey there's save data, let's load it
		//please don't actually use this, just load the seed from the Save
		//Genworld sets the seed, it will also handle loading prebuilt tiles
		GenWorld(stoi(saveName));
	}
	else {
		//World failed to load
		GenWorld(rand());
	}

	return true;
}

bool VoxelSystem::GenWorld(int seed) {
	//Generate the tile the player is standing on, the case we don't have save data it's 0,0
	GameTile * tileData = NULL;

	//this call is pointless if we use a random seed. Generator already initalizes with one
	generator->setSeed(seed);

	tileData = LoadTile(vec2(0, 0));

	//failed to load from disk cache, generate the tile
	if (tileData == NULL)
		tileData = TileHandler::getTile(vec2(0, 0));

	//this function garuntees a tile is generated so fail if it's not
	_ASSERTE(tileData != NULL);

	return true;
}

GameTile * VoxelSystem::GetTile(vec2 pos) {
	//return a tile, even if it doesn't exist yet
	GameTile * tileData = NULL;


	//search for the tile
	for (int j = 0; j < world.size(); j++) {
		if ((world[j])->tile_x == floor(pos.x / 256) && world[j]->tile_y == floor(pos.y / 256)) {
			//we found the tile
			return  world[j];
		}
	}

	tileData = LoadTile(pos);
	//failed to load from disk cache, generate the tile
	if (tileData == NULL)
		tileData = TileHandler::getTile(pos);

	//this function garuntees a tile is generated so fail if it's not
	_ASSERTE(tileData != NULL);

	return tileData;
}

GameTile * VoxelSystem::LoadTile(vec2 pos) {
	GameTile * tileData = NULL;
	stringstream s;

	//get the seed from the terrain generator
	int seed = generator->getSeed();
	s << "saves/" << seed << "-" << pos.x << "-" << pos.y << ".txt";
	tileData = GameTile::LoadTileFromDisk(s.str());

	if (tileData != NULL) {
		tileData->tile_x = pos.x;
		tileData->tile_y = pos.y;
		world.insert(tileData);
		return world[world.size() - 1];
	}

	return NULL;
}

GameTile * VoxelSystem::GenTileAsync(vec2 pos) {
	//Initialize Gametile
	GameTile * newTile = GameTile::CreateGameTile(tile_width, tile_height, pos.x, pos.y);

	//Insert Tile Into World
	int ind = world.insert(newTile);

	//Begin ASYNC Generation Thread
	std::thread genThread(VoxelSystem::GenThread, newTile);
	genThread.detach();

	//Return Tile Index in World
	return world[ind];
}

void VoxelSystem::GenThread(GameTile * newTile) {
	//Generate Terrain Data
	unsigned char* rawTile = generator->generateTile(newTile->tile_x, newTile->tile_y);
	
	//Assign Data to Container
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (tile_width * tile_height * 4));

	//Free Generated Terain Data
	free(rawTile);

	//Load Tile Data into GameTile
	GameTile::LoadTileFromMemoryIntoExisting(tile, newTile);

	//Set Ready Flag
	newTile->ready = true;
}

GameTile * VoxelSystem::GenTile(vec2 pos) {
	//Generate Terrain Tile
	unsigned char* rawTile = generator->generateTile(pos.x, pos.y);

	//Create and Fill Terrain Data Vector
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (tile_width*tile_height * 4));

	//Free Terrain Tile Memory
	free(rawTile);

	GameTile * tileData = GameTile::LoadTileFromMemory(tile, tile_width, tile_height);
	tileData->tile_x = pos.x;
	tileData->tile_y = pos.y;
	world.insert(tileData);
	return world[world.size() - 1];
}



TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	GameTile * tileData = NULL;

	//get the current tile
	tileData = GetTile(pos);

	//that better not be null or we fucked up hard
	_ASSERTE(tileData != NULL);

	//convert to relative position
	pos.x -= tileData->tile_x*256;
	pos.y -= tileData->tile_y*256;

	if ((pos.x < 0) || (pos.y < 0))
		return NULL;
	
	if ((pos.x >= tileData->Width) || (pos.y >= tileData->Height))
		return NULL;


	return tileData->GetTileCell(pos);
}

//When the voxel system calculates position height
//it will have to check which tile the position is in
//in the future. For now only one tile is loaded
float VoxelSystem::GetPositionHeight(vec2 pos) {
	TileCell * cell = GetTileCellAt(pos);

	_ASSERTE(cell != NULL);
	return cell->height + 1.0f;
}

//Get the stack size of a specific position
//all positions acceptable
int VoxelSystem::GetPositionStackSize(vec2 pos) {
	TileCell * cell = GetTileCellAt(pos);

	if (cell == NULL)
		return -1;

	//Player is within the set of valid tiles

	return cell->stackHeight + 1;
}

//intersects two ranges
//places the output into rangeA
void intersect1D(int & rangeAStart, int & rangeAEnd, int rangeBStart, int rangeBEnd) {
    //No intersection
    if ((rangeAStart > rangeBEnd) || (rangeAEnd < rangeBStart)) {
        rangeAEnd = rangeAStart;
        return;
    }
    //Find the range intersection
    rangeAStart = max(rangeAStart, rangeBStart);
    rangeAEnd = min(rangeAEnd, rangeBEnd);
}

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy) {
	//_ASSERTE(tileData != NULL);

	//Enable voxel texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	voxelCount = 0;


	//render each viewable rectangle

	for (int i = 0; i < world.size(); i++) {
        GameTile & current_tile = *world[i];
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
        rectStartX = current_tile.tile_x * 256;
        rectStartY = current_tile.tile_y * 256;
        rectEndX = rectStartX + 255;
        rectEndY = rectStartY + 255;
        //Intersect it with the region you're supposed to be drawing
        intersect1D(rectStartX, rectEndX, atx, tox);
        intersect1D(rectStartY, rectEndY, aty, toy);


        //Now offset the region by the tile position so that it is relative to the tile
        rectStartX -= current_tile.tile_x * 256;
        rectStartY -= current_tile.tile_y * 256;
        rectEndX -= current_tile.tile_x * 256;
        rectEndY -= current_tile.tile_y * 256;

		

        TileCell * cells = current_tile.Cells;

		//Skip zero length segments
		if ((rectStartY == rectEndY) || (rectStartX == rectEndX))
			continue;
		//offset rendering for tile location
		shader->Model.PushMatrix();
		shader->Model.Translate(vec3(current_tile.tile_x * 256, current_tile.tile_y * 256, 0));
		shader->Model.Apply();
		renderer->startDraw(shader);
		for (int y = rectStartY; y <= rectEndY; y++) {
			//It is important for x to be the inner loop
			//so consecutive operations access contiguous memory
			//(though performance will probably be poor anyways)
			for (int x = rectStartX; x <= rectEndX; x++) {
				//Lookup cell to render
                TileCell & cell = cells[current_tile.Width*y + x];

				//Get position
				vec3 pos = vec3(x, y, cell.height);
				int8_t stack = (int8_t)cell.stackHeight;

				voxelCount += stack + 1;

				//For now use raw tile % 16 to map all tiles to be within the 16 valid materials
				//Note: This is a really bad place for a virtual function call
				//maybe swap this out in the future
				while (stack-- >= 0) {
					renderer->pushVoxel(shader, pos, cell.materialId % 16);
					pos.z--;
				}
			}
			
		}
		renderer->finishDraw(shader);
		shader->Model.PopMatrix();
	}

}

void VoxelSystem::Update(vec3 player_pos){
	//check the players position, offload tiles, generate new tiles
	for (int x_offset = -1; x_offset <= 1; x_offset++) {
		for (int y_offset = -1; y_offset <= 1; y_offset++) {
			//make sure tile exists
			GameTile * tileData = NULL;

			//if you use gettile it will block and create a tile, search yourself
			for (int j = 0; j < world.size(); j++) {
				if ((world[j])->tile_x == floor(player_pos.x / 256) +x_offset && world[j]->tile_y == floor(player_pos.y / 256) + y_offset) {
					tileData = world[j];
					break;
				}
			}

			if (tileData == NULL) {
				//Player entered a new area

				cout << "Generating: " << floor(player_pos.x / 256) + x_offset << "," << floor(player_pos.y / 256) + y_offset << endl;
				world.insert(TileHandler::getTile(vec2(floor(player_pos.x / 256) + x_offset, floor(player_pos.y / 256) + y_offset)));
			}

		}
	}
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}

void VoxelSystem::Paint(vec2 pos, int newMaterial) {
	TileCell * cell = GetTileCellAt(pos);

	_ASSERTE(cell != NULL);

	//Player is within the set of valid tiles
	//Paint the tile material
	cell->materialId = newMaterial;
}

//Deforms a region of voxels, punching a crater into the given position
//all voxels removed are returned as positions
vector<vec4> VoxelSystem::Crater(vec3 pos, float size) {
	GameTile * current_tile;
	//render each viewable rectangle
	vector<vec4> removedVoxels;
	for (int i = 0; i < world.size(); i++) {
		current_tile = world[i];
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
		rectStartX = current_tile->tile_x * 256;
		rectStartY = current_tile->tile_y * 256;
		rectEndX = rectStartX + 255;
		rectEndY = rectStartY + 255;
		//Intersect it with the region you're supposed to be drawing
		intersect1D(rectStartX, rectEndX, pos.x - size / 2, pos.x + size / 2);
		intersect1D(rectStartY, rectEndY, pos.y - size / 2, pos.y + size / 2);


		//Now offset the region by the tile position so that it is relative to the tile
		rectStartX -= current_tile->tile_x * 256;
		rectStartY -= current_tile->tile_y * 256;
		rectEndX -= current_tile->tile_x * 256;
		rectEndY -= current_tile->tile_y * 256;

		//Skip zero length segments
		if ((rectStartY == rectEndY) || (rectStartX == rectEndX))
			continue;

		current_tile->Crater(rectStartX, rectStartY, rectEndX, rectEndY, (int)(pos.z - size / 2.0), removedVoxels);

	}
	//Return all removed voxels
	return removedVoxels;
}