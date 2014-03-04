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
bool VoxelSystem::LoadTile(string tileName) {
	//Init Terrain Generator
	TerrainGen t = TerrainGen();
	int wid = 256; int hei = 256;
	t.setTileSize(wid, hei);

	//Generate Terrain Tile
	unsigned char* rawTile = t.generateTile(0, 0);

	//Create and Fill Terrain Data Vector
	vector<unsigned char> tile;
	tile.assign(rawTile, rawTile + (wid*hei*4));

	//Free Terrain Tile Memory
	free(rawTile);

	tileData = GameTile::LoadTileFromMemory(tile,wid,hei);

	//First load the tile map
	//tileData = GameTile::LoadTileFromDisk(tileName);
	if (tileData == NULL)
		return false;

	return true;
}

TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	//Check that the position is valid
	if ((pos.x < 0) || (pos.y < 0))
		return NULL;

	if ((pos.x >= tileData->Width) || (pos.y >= tileData->Height))
		return NULL;
	return tileData->GetTile(pos);
}

//When the voxel system calculates position height
//it will have to check which tile the position is in
//in the future. For now only one tile is loaded
float VoxelSystem::GetPositionHeight(vec2 pos) {
	//The height where there is no tiles
	//right now causes the player to fall
	//because that's fun?
	static const float floorHeight = -1000.0f;

	TileCell * cell = GetTileCellAt(pos);

	if (cell == NULL)
		return floorHeight;

	//Player is within the set of valid tiles


	//Now get the height of that tile
	//we add 1 because voxels have 1 height
	//so the visible floor is 1 above the corner they draw from
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
			int8_t stack = (int8_t)cell.stackHeight;

			voxelCount += stack+1;

			//For now use raw tile % 16 to map all tiles to be within the 16 valid materials
			//Note: This is a really bad place for a virtual function call
			//maybe swap this out in the future
			while (stack-- >= 0) {
				renderer->pushVoxel(shader,pos,cell.materialId % 16);
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

void VoxelSystem::Paint(vec2 pos, int newMaterial) {
	//Check that the region is in valid space
	if ((pos.x < 0) || (pos.y < 0))
		return;

	if ((pos.x >= tileData->Width) || (pos.y >= tileData->Height))
		return;

	int x = floor(pos.x);
	int y = floor(pos.y);
	//Determine which tile to paint
	//then paint that one tile
	tileData->Cells[tileData->Width*y+x].materialId = newMaterial;
}

//Deforms a region of voxels, punching a crater into the given position
//all voxels removed are returned as positions
vector<vec4> VoxelSystem::Crater(vec3 pos, float size) {
	vector<vec4> removedVoxels;
	//Build the intersection of this crater and the valid tile(s)
	int fx = (int)(pos.x-size/2.0);
	int fy = (int)(pos.y-size/2.0);
	int tox = (int)(pos.x+size/2.0);
	int toy = (int)(pos.y+size/2.0);
	//Apply this region to the valid tile
	tileData->Crater(fx,fy,tox,toy,(int)(pos.z-size/2.0),removedVoxels);

	//Return all removed voxels
	return removedVoxels;
}