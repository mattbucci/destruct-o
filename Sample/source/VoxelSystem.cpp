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
	//for (int x = 0; x < 3; x++) {
	//	for (int y = 0; y < 3; y++) {
	//		LoadTile(seed, x, y);
	//	}
	//}
	LoadTile(seed, 0, 0);
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

	//account for negative position
	if (pos.x < 0) pos.x = 255 + pos.x;
	if (pos.y < 0) pos.y = 255 + pos.y;

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
			for (int j = 0; j < world.size(); j++) {
				if ((world[j])->tile_x == floor(player_pos.x / 256) +x_offset && world[j]->tile_y == floor(player_pos.y / 256) + y_offset) {
					tileData = world[j];
					break;
				}
			}
			if (tileData == NULL) {
				cout << "Generating: " << floor(player_pos.x / 256) + x_offset << "," << floor(player_pos.y / 256) + y_offset;
				GenTile(floor(player_pos.x / 256) + x_offset, floor(player_pos.y / 256) + y_offset);
			}

		}
	}
	
}

//Get map width
int VoxelSystem::GetWidth() {
	return tile_width * 255; //tileData->Width*5;
}
//Get map height
int VoxelSystem::GetHeight() {
	return tile_height * 255; //tileData->Height;
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

			//account for negatives
			if (current_tile.x_start < 0) current_tile.x_start = 255 + current_tile.x_start;
			if (current_tile.x_end < 0)  current_tile.x_end = 255 + current_tile.x_end;
			if (current_tile.y_start < 0)  current_tile.y_start = 255 + current_tile.y_start;
			if (current_tile.y_end < 0) current_tile.y_end = 255 + current_tile.y_end;

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