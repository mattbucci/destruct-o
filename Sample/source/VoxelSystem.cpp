#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"

#include "GL3DProgram.h"
#include "OS.h"

VoxelSystem::VoxelSystem() {
	tileWidth = tileHeight = 0;
	//Build the vertex/buffer array
	vertices = NULL;
	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenVertexArrays(1,&vertexArray);

	verticeCount = 0;
	vertices = new vec4[36];
}
VoxelSystem::~VoxelSystem() {

}

//Attempt to load a tile from disc
bool VoxelSystem::LoadTile(string tileName) {
	//First load the tile map
	if (lodepng::decode(rawTile,tileWidth,tileHeight,tileName)) {
		//Error
		return false;
	}
	//Next load the voxel textures
	vector<unsigned char> image;
	unsigned int textureWidth;
	unsigned int textureHeight;
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


	return true;
}

void VoxelSystem::pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber,int materialId) {
	//The 4th float in each vector is a vertex id mixed with the material id
	//and is used to generate normals and texture coordinates on the gpu
	vertices[vertNumber++] = vec4(a+pos,(float)(vertNumber+materialId*1000));
	vertices[vertNumber++] = vec4(b+pos,(float)(vertNumber+materialId*1000));
	vertices[vertNumber++] = vec4(c+pos,(float)(vertNumber+materialId*1000));
	vertices[vertNumber++] = vec4(b+pos,(float)(vertNumber+materialId*1000));
	vertices[vertNumber++] = vec4(d+pos,(float)(vertNumber+materialId*1000));
	vertices[vertNumber++] = vec4(c+pos,(float)(vertNumber+materialId*1000));
}

void VoxelSystem::pushVoxel(vec3 pos,int materialId, int & vertNumber) {

	//So the original plan was to use one buffer that's why we have vertNumber
	//but I ran out of time
	vertNumber = 0;

	//A cube

	//Top
	pushSide(pos,vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertNumber,materialId);
	//Bottom
	pushSide(pos,vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertNumber,materialId);
	//Left
	pushSide(pos,vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertNumber,materialId);
	//Right
	pushSide(pos,vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertNumber,materialId);
	//Back
	pushSide(pos,vec3(0,1,0),vec3(0,1,0),vec3(0,1,1),vec3(1,1,0),vec3(1,1,1),vertNumber,materialId);
	//Front
	pushSide(pos,vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertNumber,materialId);

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	//Push voxel to gpu
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec4), vertices, GL_DYNAMIC_READ );
	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
	
	//Draw voxel
	glDrawArrays( GL_TRIANGLES, 0, 36 );
}

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader,vec3 drawPos, int atx, int aty, int tox, int toy) {
	_ASSERTE(atx >= 0);
	_ASSERTE(aty >= 0);
	_ASSERTE(tox < (int)tileWidth);
	_ASSERTE(toy < (int)tileHeight);

	//Enable voxel texture
	glBindTexture(GL_TEXTURE_2D,textureId);

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

			//For now use raw tile % 2 to map all tiles to be within the 2 valid materials
			//that i've made
			pushVoxel(pos,rawTile[pixelNumber+1] % 2,vertNumber);
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