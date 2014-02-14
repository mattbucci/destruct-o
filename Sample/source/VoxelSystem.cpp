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

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED

#ifndef __MOBILE__
	glGenerateMipmap(GL_TEXTURE_2D);
#endif
}
VoxelSystem::~VoxelSystem() {
	delete renderer;
	delete tileData;
}

//Attempt to load a tile from disc
bool VoxelSystem::LoadTile(string tileName) {
	/*//Init Terrain Generator
	TerrainGen t = TerrainGen();
	int wid = 1024; int hei = 1024;
	t.setSeed(2);
	t.setTileScale(1, 1);
	t.setTileSize(wid, hei);

	//Generate Terrain Tile
	unsigned char* rawtile = t.generateTile(0, 0);

	//Create and Fill Terrain Data Vector
	vector<unsigned char> tile;
	int s = wid * hei;
	tile.resize(s * 4);
	for(int i = 0; i < s; i++) {
		tile[(i*4)] = rawtile[i];
		tile[(i*4) + 1] = 0;
		tile[(i*4) + 2] = 0;
		tile[(i*4) + 3] = 0;
	}

	//Free Terrain Tile Memory
	free(rawtile);*/

	//Generate a flat 1024x1024 tile for physics testing
	vector<unsigned char> flat(1024*1024*4);
	memset(&flat.front(),0,1024*1024*4);

	tileData = GameTile::LoadTileFromMemory(flat,1024,1024);

	//First load the tile map
	//tileData = GameTile::LoadTileFromDisk(tileName);
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
	static const float floorHeight = -1000.0f;

	if ((pos.x < 0) || (pos.y < 0))
		return floorHeight;

	if ((pos.x >= tileData->Width) || (pos.x >= tileData->Height))
		return floorHeight;

	//Player is within the set of valid tiles
	return tileData->GetHeight(pos);
}

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader,vec3 drawPos, int atx, int aty, int tox, int toy) {
	_ASSERTE(atx >= 0);
	_ASSERTE(aty >= 0);
	_ASSERTE(tileData != NULL);
	_ASSERTE(tox < (int)tileData->Width);
	_ASSERTE(toy < (int)tileData->Height);

	//Enable voxel texture
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureId);

	voxelCount = 0;
	
	renderer->startDraw(shader);

	TileCell * cells = tileData->Cells;

	for (int y = aty; y <= toy; y++) {
		//It is important for x to be the inner loop
		//so consecutive operations access contiguous memory
		//(though performance will probably be poor anyways)
		for (int x = atx; x <= tox; x++) {
			//Lookup cell to render
			TileCell & cell = cells[tileData->Width*y+x];

			//Get position
			vec3 pos = vec3(x,y,cell.height);
			char stack = (char)cell.stackHeight;

			voxelCount += stack+1;

			//For now use raw tile % 2 to map all tiles to be within the 2 valid materials
			//that i've made
			//Note: This is a really bad place for a virtual function call
			//maybe swap this out in the future
			while (stack-- >= 0) {
				renderer->pushVoxel(shader,pos,cell.materialId % 2);
				pos.z--;
			}
		}
	}

	renderer->finishDraw(shader);
}

//Get map width
int VoxelSystem::GetWidth() {
	return tileData->Width;
}
//Get map height
int VoxelSystem::GetHeight() {
	return tileData->Height;
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}
