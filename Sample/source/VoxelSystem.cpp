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

const int tilesize = 1024;

VoxelSystem::VoxelSystem() {
	renderer = VoxelDrawSystem::BuildAppropriateSystem();
	tiles = new TileHandler();
	tiles->init();

	//Just get the starting tile
	tiles->getTile(vec2(0, 0));

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

GameTile * VoxelSystem::GetTile(vec2 pos) {
	//TileHandler Guaranteed to Return Tile
	return tiles->getTile(pos);
}

TileCell * VoxelSystem::GetTileCellAt(vec2 pos) {
	GameTile * tileData = GetTile(vec2(floor(pos.x / tilesize), floor(pos.y / tilesize)));

	//Convert to relative position
	pos.x -= tileData->tile_x*tilesize;
	pos.y -= tileData->tile_y*tilesize;

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

void VoxelSystem::forTilesInRect(Rect region, function<void(GameTile * tile)> foreachFunction) {
	float xend = region.X + region.Width;
	float yend = region.Y + region.Height;
	for (float x = region.X;; x+=tilesize) {
		for (float y = region.Y;; y+=tilesize) {
			//Retrieve a valid tile for this position
			//and call the foreachFunction
			foreachFunction(GetTile(glm::floor(vec2(x/tilesize,y/tilesize))));

			//Exit only after an iteration past the end
			if (y >= yend)
				break;
		}
		//Exit only after an iteration past the end
		if (x >= xend)
			break;
	}
}

//intersects two ranges
//places the output into rangeA
static void intersect1D(int & rangeAStart, int & rangeAEnd, int rangeBStart, int rangeBEnd) {
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

	forTilesInRect(Rect((float)atx,(float)aty,(float)(tox-atx),(float)(toy-aty)),[this,shader,atx,aty,tox,toy](GameTile * tile) {
		GameTile & current_tile = *tile;
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
		rectStartX = current_tile.tile_x * tilesize;
		rectStartY = current_tile.tile_y * tilesize;
		rectEndX = rectStartX + tilesize-1;
		rectEndY = rectStartY + tilesize-1;
		//Intersect it with the region you're supposed to be drawing
		intersect1D(rectStartX, rectEndX, atx, tox);
		intersect1D(rectStartY, rectEndY, aty, toy);


		//Now offset the region by the tile position so that it is relative to the tile
		rectStartX -= current_tile.tile_x * tilesize;
		rectStartY -= current_tile.tile_y * tilesize;
		rectEndX -= current_tile.tile_x * tilesize;
		rectEndY -= current_tile.tile_y * tilesize;

		TileCell * cells = current_tile.Cells;

		//Skip zero length segments
		if ((rectStartY == rectEndY) || (rectStartX == rectEndX))
			return;
		//offset rendering for tile location
		shader->Model.PushMatrix();
		shader->Model.Translate(vec3(current_tile.tile_x * tilesize, current_tile.tile_y * tilesize, 0));
		shader->Model.Apply();

		//First draw the voxels
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
		

		//Next check for any structures on this tile which intersect the view rectangle
		for (auto structure : current_tile.Structures) {
			int structx = (int)structure.Position.x;
			int structy = (int)structure.Position.y;
			int structex = structx + (int)structure.Extents.x;
			int structey = structx + (int)structure.Extents.x;
			//Now see if the struct rectangle intersects the draw rectangle
			if ((rectStartX < structex) ||
				(rectEndX > structx) ||
				(rectStartY < structey) ||
				(rectEndY > structy)) {
				//Time to draw the structure
				//Push the structure's position
				if (structure.Cells.size() > 0){
					shader->Model.PushMatrix();
					shader->Model.Translate(structure.Position);
					shader->Model.Apply();
					//Track voxels drawn for debug
					voxelCount += structure.Cells.size();
					//Start the draw cycle
					renderer->startDraw(shader);
					StructureCell * celliterator = &structure.Cells.front();
					unsigned int endCount = structure.Cells.size();
					//Push all the cells
					for (unsigned int i = 0; i < endCount; i++, celliterator++)
						renderer->pushVoxel(shader, celliterator->pos, celliterator->material);
					//Finish drawing and remove the structure matrix
					renderer->finishDraw(shader);
					shader->Model.PopMatrix();
				}
			}
		}
		shader->Model.PopMatrix();

		
	});

}

void VoxelSystem::Update(vec3 player_pos){
	//Pass to TileHandler
	tiles->getTile(vec2(floor(player_pos.x / tilesize), floor(player_pos.y / tilesize)));
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
	//render each viewable rectangle
	vector<vec4> removedVoxels;
	forTilesInRect(Rect(pos.x - size / 2,pos.y - size / 2,size,size),[this,&removedVoxels,pos,size](GameTile * current_tile) {
		int rectStartX, rectStartY, rectEndX, rectEndY;
		//Get the region this tile is in
		rectStartX = current_tile->tile_x * tilesize;
		rectStartY = current_tile->tile_y * tilesize;
		rectEndX = rectStartX + tilesize-1;
		rectEndY = rectStartY + tilesize-1;
		//Intersect it with the region you're supposed to be drawing
		intersect1D(rectStartX, rectEndX, pos.x - size / 2, pos.x + size / 2);
		intersect1D(rectStartY, rectEndY, pos.y - size / 2, pos.y + size / 2);


		//Now offset the region by the tile position so that it is relative to the tile
		rectStartX -= current_tile->tile_x * tilesize;
		rectStartY -= current_tile->tile_y * tilesize;
		rectEndX -= current_tile->tile_x * tilesize;
		rectEndY -= current_tile->tile_y * tilesize;

		//Skip zero length segments
		if ((rectStartY == rectEndY) || (rectStartX == rectEndX))
			return;

		current_tile->Crater(rectStartX, rectStartY, rectEndX, rectEndY, (int)(pos.z - size / 2.0), removedVoxels);

	});
	//Return all removed voxels
	return removedVoxels;
}


