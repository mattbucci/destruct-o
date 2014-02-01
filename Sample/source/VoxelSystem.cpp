#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"

VoxelSystem::VoxelSystem() {
	//Resize self to match expected vertex count
	UpdateGroup(GL_TRIANGLES,36);
	tileWidth = tileHeight = 0;
}
VoxelSystem::~VoxelSystem() {

}

//Attempt to load a tile from disc
bool VoxelSystem::LoadTile(string tileName) {
	if (lodepng::decode(rawTile,tileWidth,tileHeight,tileName)) {
		//Error
		return false;
	}
	return true;
	/*//I should have moved the png->texture into a utility library
	//later...
	glGenTextures( 1, (GLuint*)&openglTextureId );
	glBindTexture(GL_TEXTURE_2D, openglTextureId);


	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tileWidth,
				tileHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				&image[0] );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//GL_NEAREST FOR SPEED
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//GL_NEAREST FOR SPEED*/
}

void VoxelSystem::pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber) {
	nat(vertNumber) = normal;
	vat(vertNumber++) = a+pos;
	nat(vertNumber) = normal;
	vat(vertNumber++) = b+pos;
	nat(vertNumber) = normal;
	vat(vertNumber++) = c+pos;
	nat(vertNumber) = normal;
	vat(vertNumber++) = d+pos;
	nat(vertNumber) = normal;
	vat(vertNumber++) = b+pos;
	nat(vertNumber) = normal;
	vat(vertNumber++) = c+pos;
}

void VoxelSystem::pushVoxel(vec3 pos,int materialId, int & vertNumber) {

	//So the original plan was to use one buffer that's why we have vertNumber
	//but I ran out of time
	vertNumber = 0;

	//A cube

	//Top
	pushSide(pos,vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Bottom
	pushSide(pos,vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertNumber);
	//Left
	pushSide(pos,vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Right
	pushSide(pos,vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertNumber);
	//Back
	pushSide(pos,vec3(0,1,0),vec3(0,1,0),vec3(1,1,0),vec3(0,1,1),vec3(1,1,1),vertNumber);
	//Front
	pushSide(pos,vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertNumber);

	GLDynamicVertexGroup::Draw();
}

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader,vec3 drawPos, int atx, int aty, int tox, int toy) {
	_ASSERTE(atx >= 0);
	_ASSERTE(aty >= 0);
	_ASSERTE(tox < tileWidth);
	_ASSERTE(toy < tileHeight);

	int vertNumber = 0;
	

	for (int y = aty; y <= toy; y++) {
		//It is important for x to be the inner loop
		//so consecutive operations access contiguous memory
		//(though performance will probably be poor anyways)
		for (int x = atx; x <= tox; x++) {
			//Lookup pixel number
			int pixelNumber = (tileWidth*y+x)*4;
			//Get position
			vec3 pos = vec3(x,y,rawTile[pixelNumber]/10);
			//Calculate distance squared
			vec3 dist = drawPos-pos;
			//Reject if too far away (500 tiles hard coded right now)
			if (glm::dot(dist,dist) < 100*100)
				//For now use raw tile % 4 to map all tiles to be within the 4 valid materials
				//that i've made
				pushVoxel(pos,rawTile[pixelNumber+1] % 4,vertNumber);
		}
	}
}

//Get map width
int VoxelSystem::GetWidth() {
	return tileWidth;
}
//Get map height
int VoxelSystem::GetHeight() {
	return tileHeight;
}