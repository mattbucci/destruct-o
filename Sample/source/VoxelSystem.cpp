#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"

#include "GL3DProgram.h"
#include "OS.h"

#include "GameTile.h"

#include "VoxelDrawSystem.h"
#include "InstancedVoxelRenderSystem.h"
#include "BasicVoxelRenderSystem.h"

VoxelSystem::VoxelSystem() {
	//No tile data loaded
	tileData = NULL;

	//Choose the appropriate render version
	switch (OpenglVersion){
	case 20:
#if (defined __IPHONEOS__)
        // Always use instanced renderer on iOS for OpenGL ES 2.0
        renderer = new InstancedVoxelRenderSystem();
#else
		renderer = new BasicVoxelRenderSystem();
#endif
		break;
//Android can't use instanced rendering so its not even an option
#ifndef __ANDROID__
	case 31:
		renderer = new InstancedVoxelRenderSystem();
		break;
#endif
    }
}
VoxelSystem::~VoxelSystem() {
	delete renderer;
	delete tileData;
}

//Attempt to load a tile from disc
bool VoxelSystem::LoadTile(string tileName) {
	//First load the tile map
	tileData = GameTile::LoadTileFromDisk(tileName);
	if (tileData == NULL)
		return false;

	unsigned int textureWidth, textureHeight;
	vector<unsigned char> image;
	
	//A smart system would have multiple res tiles and would automatically select
	//one appropriate for the system its running on
	if (lodepng::decode(image,textureWidth,textureHeight,"terrain/tiles-lowres.png")) {
		//Error
		return false;
	}

	//I should have moved the png->texture into a utility library
	//later...
	glGenTextures( 1, &textureId );
	glBindTexture(GL_TEXTURE_2D, textureId);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textureWidth,
				textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&image[0] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_NEAREST FOR SPEED

	cout << "Cached tile texture <" << textureWidth << "," << textureHeight << "> with id " << textureId << "\n";

	return true;
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

			//For now use raw tile % 2 to map all tiles to be within the 2 valid materials
			//that i've made
			//Note: This is a really bad place for a virtual function call
			//maybe swap this out in the future
			while (stack-- >= 0) {
				renderer->pushVoxel(shader,pos,cell.materialId % 2);
				pos.z--;
			}
			voxelCount++;
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
