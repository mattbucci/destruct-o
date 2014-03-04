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

VoxelSystem::VoxelSystem() {
	//No tile data loaded
	tileData = NULL;

	renderer = VoxelDrawSystem::BuildAppropriateSystem();

	//Load the tile textures
	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;

	//A smart system would have multiple res tiles and would automatically select
	//one appropriate for the system its running on
	lodepng::decode(image,textureWidth,textureHeight,"terrain/tiles-lowres.png");

	//I should have moved the png->texture into a utility library
	//later...
	glGenTextures( 1, &textureId );
	glBindTexture(GL_TEXTURE_2D, textureId);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth,
				textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&image[0] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
}
VoxelSystem::~VoxelSystem() {
	delete renderer;
	delete tileData;
}

//Attempt to load a tile from disc
bool VoxelSystem::LoadWorld(string saveName) {
	//First load the tile map
	tileData = GameTile::LoadTileFromDisk(saveName);
	if (tileData == NULL)
		return false;

	return true;
}

bool VoxelSystem::GenWorld() {
	for (int i = 0; i < 5; i++) {
		//Init Terrain Generator
		TerrainGen t = TerrainGen();
		int wid = 256; int hei = 256;
		t.setTileSize(wid, hei);

		//Generate Terrain Tile
		unsigned char* rawTile = t.generateTile(i, 0);

		//Create and Fill Terrain Data Vector
		vector<unsigned char> tile;
		tile.assign(rawTile, rawTile + (wid*hei * 4));

		//Free Terrain Tile Memory
		free(rawTile);

		tileData = GameTile::LoadTileFromMemory(tile, wid, hei);
		tileData->tile_x = i;
		tileData->tile_y = 0;
		world.insert(tileData);
	}
		//First load the tile map
		if (tileData == NULL)
			return false;

		return true;
}

//When the voxel system calculates position height
//it will have to check which tile the position is in
//in the future. For now only one tile is loaded
float VoxelSystem::GetPositionHeight(vec2 pos) {
	//The height where there is no tiles
	//right now causes the player to fall
	//because that's fun?
	tileData = NULL;
	static const float floorHeight = -1000.0f;

	for (int j = 0; j < world.size(); j++) {
		if ((world[j])->tile_x == floor(pos.x / 256) && world[j]->tile_y == floor(pos.y / 256)) {
			tileData = world[j];
			break;
		}
	}
	if (tileData != NULL){
		
		if ((pos.x < 0) || (pos.y < 0))
			return floorHeight;

		//if ((pos.x >= tileData->Width) || (pos.y >= tileData->Height))
		//	return floorHeight;

		//Player is within the set of valid tiles
		return tileData->GetHeight(vec2(int(pos.x) % 256,int(pos.y) % 256));
	}
	return floorHeight;
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
	cout << "Player Position: "<< atx << " : " << tox << "," << aty << " : " << toy <<  endl;

	//break the view into relevant rectangles
	for (int y = aty; y <= toy; y += 256 - y % 256) {
		//start at the smallest value and get tile aligned asap
		for (int x = atx; x <= tox; x += 256 - x % 256) {
			//calculate offsets for this rectangle
			current_tile.tile_x = floor(x / 256);
			current_tile.tile_y = floor(y / 256);

			current_tile.x_start = x % 256;
			current_tile.x_end = (min(x + 256 - x % 256, tox)-1) % 256;
			current_tile.y_start = y % 256;
			current_tile.y_end = (min(y + 256 - y % 256, toy) - 1) % 256;
			tiles.push_back(current_tile);
			cout << "tile: " << current_tile.x_start << ":" << current_tile.x_end <<" , " << current_tile.y_start << ":" << current_tile.y_end << endl;

		}
	}
	voxelCount = 0;

	
	//render each viewable rectangle

	for (int i = 0; i < tiles.size(); i++) {
		current_tile = tiles[i];
		//find the tile in our world
		tileData = NULL;
		cout << current_tile.tile_x << " : " << current_tile.tile_y << endl;
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
			cout << "transform:" << current_tile.tile_x * 256 << ":" << current_tile.tile_y * 256 << endl;
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

//Get map width
int VoxelSystem::GetWidth() {
	return 1024; //tileData->Width*5;
}
//Get map height
int VoxelSystem::GetHeight() {
	return 256; //tileData->Height;
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}
