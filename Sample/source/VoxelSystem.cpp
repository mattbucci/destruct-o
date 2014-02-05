#include "stdafx.h"
#include "VoxelSystem.h"
#include "lodepng.h"

#include "GL3DProgram.h"
#include "OS.h"

VoxelSystem::VoxelSystem() {
	tileWidth = tileHeight = 0;
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

	cout << "Cached tile texture <" << textureWidth << "," << textureHeight << "> with id " << textureId << "\n";

	return true;
}

//Draw the voxels in a region
void VoxelSystem::Draw(GL3DProgram * shader,vec3 drawPos, int atx, int aty, int tox, int toy) {
	_ASSERTE(atx >= 0);
	_ASSERTE(aty >= 0);
	_ASSERTE(tox < (int)tileWidth);
	_ASSERTE(toy < (int)tileHeight);

	//Enable voxel texture
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureId);

	int vertNumber = 0;
	voxelCount = 0;
	
	renderer.startDraw(shader);

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
			//pushVoxel(pos,rawTile[pixelNumber+1] % 2,vertNumber);
			renderer.pushVoxel(shader,pos,rawTile[pixelNumber+1] % 2);
			voxelCount++;
		}
	}

	renderer.finishDraw(shader);
}

//Get map width
int VoxelSystem::GetWidth() {
	return tileWidth;
}
//Get map height
int VoxelSystem::GetHeight() {
	return tileHeight;
}

int VoxelSystem::GetLastVoxelCount() {
	return voxelCount;
}


//Render system follows:
#ifdef __ANDROID__
//TODO: Do this render system (use the old method with the new class)

#else
VoxelSystem::InstancedRenderSystem::InstancedRenderSystem() {
	glGenBuffers(1,&vertexBuffer);
	glGenBuffers(1,&textureBuffer);
	glGenBuffers(1,&normalBuffer);
	glGenBuffers(1,&positionBuffer);
	glGenVertexArrays(1,&vertexArray);

	bufferedVoxels = 0;
	vertices = new vec3[36];
	textureCoordinates = new vec2[36];
	normals = new vec3[36];
	positions = new vec4[INSTANCE_RENDER_SWEEP];

	//Build the cube just this once
	int vertNumber = 0;

	//A cube

	//Top
	pushSide(vec3(0,0,1),vec3(0,0,1),vec3(0,1,1),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Bottom
	pushSide(vec3(0,0,-1),vec3(0,0,0),vec3(0,1,0),vec3(1,0,0),vec3(1,1,0),vertNumber);
	//Left
	pushSide(vec3(1,0,0),vec3(1,0,0),vec3(1,1,0),vec3(1,0,1),vec3(1,1,1),vertNumber);
	//Right
	pushSide(vec3(-1,0,0),vec3(0,0,0),vec3(0,1,0),vec3(0,0,1),vec3(0,1,1),vertNumber);
	//Back
	pushSide(vec3(0,1,0),vec3(0,1,0),vec3(0,1,1),vec3(1,1,0),vec3(1,1,1),vertNumber);
	//Front
	pushSide(vec3(0,-1,0),vec3(0,0,0),vec3(1,0,0),vec3(0,0,1),vec3(1,0,1),vertNumber);

	//Generate the sequential indices
	for (int i = 0; i < 36; i++)
		indices.push_back(i);

	allocated = false;
}

VoxelSystem::InstancedRenderSystem::~InstancedRenderSystem() {
	delete [] vertices;
	delete [] textureCoordinates;
	delete [] normals;
	delete [] positions;
}

void VoxelSystem::InstancedRenderSystem::pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber) {
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,0);
	vertices[vertNumber++] = a;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,0);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,1);
	vertices[vertNumber++] = c;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,0);
	vertices[vertNumber++] = b;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(1,1);
	vertices[vertNumber++] = d;
	normals[vertNumber] = normal;
	textureCoordinates[vertNumber] = vec2(0,1);
	vertices[vertNumber++] = c;
}

//Called at the start of the draw cycle
//pushes the cube vertices/normals/texture coordinates to the gpu
void VoxelSystem::InstancedRenderSystem::startDraw(GL3DProgram * shader) {
	//Only run gpu allocation code once
	if (allocated)
		return;

	allocated = true;

	//Allocate the space for the gpu buffers now
	//and send the static data
	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	//Push voxel to gpu
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), vertices, GL_STATIC_READ );
	glEnableVertexAttribArray ( shader->AttributeVertex() );
	glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer ( GL_ARRAY_BUFFER, textureBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec2), textureCoordinates, GL_STATIC_READ );
	glEnableVertexAttribArray ( shader->AttributeTexture() );
	glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, 0, 0 );
	
	glBindBuffer ( GL_ARRAY_BUFFER, normalBuffer );
	glBufferData ( GL_ARRAY_BUFFER, 36*sizeof(vec3), normals, GL_STATIC_READ );
	glEnableVertexAttribArray ( shader->AttributeNormal() );
	glVertexAttribPointer ( shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, 0, 0 );
	//Allocate space for positions
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferData ( GL_ARRAY_BUFFER, INSTANCE_RENDER_SWEEP*sizeof(vec4), positions, GL_DYNAMIC_READ );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );	
}

void VoxelSystem::InstancedRenderSystem::draw(GL3DProgram * shader) {

	glBindVertexArray ( vertexArray );
	glBindBuffer ( GL_ARRAY_BUFFER, positionBuffer );
	glBufferSubData ( GL_ARRAY_BUFFER, 0,INSTANCE_RENDER_SWEEP*sizeof(vec4), positions );
	glEnableVertexAttribArray ( shader->AttributePosition() );
	glVertexAttribPointer ( shader->AttributePosition(), 4, GL_FLOAT, GL_FALSE, 0, 0 );	

	//The position is per-instance
	//everything else is per-vertex
	glVertexAttribDivisor(shader->AttributeNormal(),0);
	glVertexAttribDivisor(shader->AttributePosition(),1);
	glVertexAttribDivisor(shader->AttributeTexture(),0);
	glVertexAttribDivisor(shader->AttributeVertex(),0);

	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, &indices[0], bufferedVoxels);
	//All buffered voxels now drawn
	bufferedVoxels = 0;
}

//Add a voxel and maybe draw
//Called per voxel
void VoxelSystem::InstancedRenderSystem::pushVoxel(GL3DProgram * shader, vec3 pos,int materialId) {
	positions[bufferedVoxels++] = vec4(pos,(float)materialId);

	if (bufferedVoxels == INSTANCE_RENDER_SWEEP) {
		draw(shader);
	}
}

//Called at the end of the draw cycle
//draws any undrawn voxels
void VoxelSystem::InstancedRenderSystem::finishDraw(GL3DProgram * shader) {
	if (bufferedVoxels > 0)
		draw(shader);
}
#endif