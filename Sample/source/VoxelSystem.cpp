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

static const int tile_width = 256;
static const int tile_height = 256;

VoxelSystem::VoxelSystem() {
	//Init Terrain Generator
	generator = new TerrainGen();
	generator->setTileSize(tile_width, tile_height);

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

//Attempt to load a tile from disc
bool VoxelSystem::LoadWorld(string saveName) {
	if (0) {

	}
	else {
		//todo, set seed
		GenWorld(128);

	}

	return true;
}

bool VoxelSystem::LoadTile(int seed, int x, int y) {
	stringstream s;
	s << "saves/" << seed << "-" << x << "-" << y << ".txt";
	GameTile * tileData = GameTile::LoadTileFromDisk(s.str());

	if (tileData != NULL) {
		tileData->tile_x = x;
		tileData->tile_y = y;
		world.insert(tileData);
		return true;

	}
	else {
		GenTile(x, y);
		return false;
	}

}

bool VoxelSystem::GenTile(int x, int y) {
	//Generate Terrain Tile
	unsigned char* rawTile = generator->generateTile(x, y);

	//Create and Fill Terrain Data Vector
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (tile_width*tile_height * 4));

	//Free Terrain Tile Memory
	free(rawTile);

	GameTile * tileData = GameTile::LoadTileFromMemory(tile, tile_width, tile_height);
	tileData->tile_x = x;
	tileData->tile_y = y;
	world.insert(tileData);
	return true;
}


bool VoxelSystem::GenWorld(int seed) {
	//todo, call set seed
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			LoadTile(seed, x, y);
		}
	}

	return true;
}

TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	GameTile * tileData = NULL;

	//Check that the position is valid
	for (int j = 0; j < world.size(); j++) {
		if ((world[j])->tile_x == floor(pos.x / 256) && world[j]->tile_y == floor(pos.y / 256)) {
			tileData = world[j];
			break;
		}
	}
	//convert to relative position
	pos.x = int(pos.x) % 256;
	pos.y = int(pos.y) % 256;

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
	//The height where there is no tiles
	//right now causes the player to fall
	//because that's fun?
	GameTile * tileData = NULL;
	static const float floorHeight = -1000.0f;

	for (int j = 0; j < world.size(); j++) {
		if ((world[j])->tile_x == floor(pos.x / 256) && world[j]->tile_y == floor(pos.y / 256)) {
			tileData = world[j];
			break;
		}
	}
	TileCell * cell = GetTileCellAt(pos);

	if (tileData != NULL){

		
		if (cell == NULL)
			return floorHeight;

		//Player is within the set of valid tiles


		//Now get the height of that tile
		//we add 1 because voxels have 1 height
		//so the visible floor is 1 above the corner they draw from
		return cell->height + 1.0f;
	}
	return  floorHeight;
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

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy) {
	_ASSERTE(atx >= 0);
	_ASSERTE(aty >= 0);
	//_ASSERTE(tileData != NULL);

	//Enable voxel texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	vector<tile_renderinfo> tiles;
	tile_renderinfo current_tile;
	//cout << "Player Position: " << atx << " : " << tox << "," << aty << " : " << toy << endl;

	//break the view into relevant rectangles
	for (int y = aty; y <= toy; y += 256 - y % 256) {
		//start at the smallest value and get tile aligned asap
		for (int x = atx; x <= tox; x += 256 - x % 256) {
			//calculate offsets for this rectangle
			current_tile.tile_x = floor(x / 256);
			current_tile.tile_y = floor(y / 256);

			current_tile.x_start = x % 256;
			current_tile.x_end = (min(x + 256 - x % 256, tox) - 1) % 256;
			current_tile.y_start = y % 256;
			current_tile.y_end = (min(y + 256 - y % 256, toy) - 1) % 256;
			tiles.push_back(current_tile);

		}
	}
	voxelCount = 0;


	//render each viewable rectangle

	for (int i = 0; i < tiles.size(); i++) {
		current_tile = tiles[i];
		//find the tile in our world
		GameTile * tileData = NULL;
		for (int j = 0; j < world.size(); j++) {
			if ((world[j])->tile_x == current_tile.tile_x && world[j]->tile_y == current_tile.tile_y) {
				tileData = world[j];
				break;
			}
		}
		if (tileData != NULL){
			shader->Model.PushMatrix();
			shader->Model.Translate(vec3(current_tile.tile_x * 256, current_tile.tile_y * 256, 0));
			shader->Model.Apply();
			TileCell * cells = tileData->Cells;


			renderer->startDraw(shader);
			for (int y = current_tile.y_start; y <= current_tile.y_end; y++) {
				//It is important for x to be the inner loop
				//so consecutive operations access contiguous memory
				//(though performance will probably be poor anyways)
				for (int x = current_tile.x_start; x <= current_tile.x_end; x++) {
					//Lookup cell to render
					TileCell & cell = cells[tileData->Width*y + x];

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

}

void VoxelSystem::Update(vec2 player_pos){
	//check the players position, offload tiles, generate new tiles
}

//Get map width
int VoxelSystem::GetWidth() {
	return tile_width * 3; //tileData->Width*5;
}
//Get map height
int VoxelSystem::GetHeight() {
	return tile_height * 3; //tileData->Height;
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}

void VoxelSystem::Paint(vec2 pos, int newMaterial) {
	TileCell * cell = GetTileCellAt(pos);

	if (cell == NULL)
		return;

	//Player is within the set of valid tiles
	//Paint the tile material
	cell->materialId = newMaterial;
}

//Deforms a region of voxels, punching a crater into the given position
//all voxels removed are returned as positions
vector<vec4> VoxelSystem::Crater(vec3 pos, float size) {
	GameTile * tileData;

	vector<tile_renderinfo> tiles;
	tile_renderinfo current_tile;
	//get the crater sides
	int atx = (int)(pos.x - size / 2.0);
	int aty = (int)(pos.y - size / 2.0);
	int tox = (int)(pos.x + size / 2.0);
	int toy = (int)(pos.y + size / 2.0);

	//break the view into relevant rectangles
	for (int y = aty; y <= toy; y += 256 - y % 256) {
		//start at the smallest value and get tile aligned asap
		for (int x = atx; x <= tox; x += 256 - x % 256) {
			//calculate offsets for this rectangle
			current_tile.tile_x = floor(x / 256);
			current_tile.tile_y = floor(y / 256);

			current_tile.x_start = x % 256;
			current_tile.x_end = (min(x + 256 - x % 256, tox) - 1) % 256;
			current_tile.y_start = y % 256;
			current_tile.y_end = (min(y + 256 - y % 256, toy) - 1) % 256;
			tiles.push_back(current_tile);

		}
	}
	vector<vec4> removedVoxels;

	for (int i = 0; i < tiles.size(); i++) {
		current_tile = tiles[i];
		//find the tile in our world
		GameTile * tileData = NULL;
		for (int j = 0; j < world.size(); j++) {
			if ((world[j])->tile_x == current_tile.tile_x && world[j]->tile_y == current_tile.tile_y) {
				tileData = world[j];
				break;
			}
		}
		if (tileData != NULL){
			for (int y = current_tile.y_start; y <= current_tile.y_end; y++) {
				for (int x = current_tile.x_start; x <= current_tile.x_end; x++) {

					//Modify pos to be relative to the tile
					pos -= vec3((float)tileData->tile_x*tile_width, (float)tileData->tile_y*tile_height, 0.0f);
					//Build the intersection of this crater and the valid tile(s)

					//Apply this region to the valid tile
					tileData->Crater(current_tile.x_start, current_tile.y_start, current_tile.x_end, current_tile.y_end, (int)(pos.z - size / 2.0), removedVoxels);
					
				}
			}
		}
	}
	//Return all removed voxels
	return removedVoxels;
}