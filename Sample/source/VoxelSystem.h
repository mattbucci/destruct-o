#pragma once

#include "stdafx.h"

class GL3DProgram;

class VoxelSystem {
	unsigned int tileWidth;
	unsigned int tileHeight;
	//Stores the tile in RGBA format 
	vector<unsigned char> rawTile;
	
	//A debug counter for the number of voxels rendered last frame
	int voxelCount;

	//The voxel texture
	GLuint textureId;

	//Different voxel systems are used for android vs. all other platforms
#ifdef __ANDROID__
	class AndroidRenderSystem {
		//Push a side of the cube
		//Used 6 times for each voxel
		void pushSide(vec3 pos, vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber,int materialId);

		GLuint vertexArray;
		//Normals and texture coordinates
		//generated inside the shader
		GLuint vertexBuffer;
		//vertices are 4 floats instead of 3 because the 4th is an id
		//which allows normals and texture coordinate generation
		vec4 * vertices;
	public:
		AndroidRenderSystem();
		~AndroidRenderSystem();

		//Called at the start of the draw cycle
		//pushes the cube vertices/normals/texture coordinates to the gpu
		void startDraw(GL3DProgram * shader);

		//Draw one voxel now
		//Called per voxel
		void pushVoxel(GL3DProgram * shader, vec3 pos,int materialId, int & vertNumber);

		//Called at the end of the draw cycle
		//draws any undrawn voxels
		void finishDraw(GL3DProgram * shader);
	};
	typedef AndroidRenderSystem RenderSystem;
#else
//Defines how many instances will be rendered (max) per
//draw call
#define INSTANCE_RENDER_SWEEP 200
	class InstancedRenderSystem {
		//Push a side of the cube
		//Only used during initial construction of the cube during the construction of the instanced render system
		void pushSide(vec3 normal, vec3 a, vec3 b, vec3 c, vec3 d, int & vertNumber);

		//Opengl buffers
		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint textureBuffer;
		GLuint normalBuffer;
		//Used for instanced positions
		GLuint positionBuffer;

		//The cpu-side buffers
		vec3 * vertices;
		vec2 * textureCoordinates;
		vec3 * normals;
		vec4 * positions; //the 4th float is the material id
		vector<GLubyte> indices;
		//How many voxels are in the buffers right now
		int bufferedVoxels;
		//Is false until the gpu buffers have been
		//allocated during the first draw cycle
		bool allocated;

		//Flushes the CPU side buffer to the gpu using instanced rendering
		void draw(GL3DProgram * shader);
	public:
		InstancedRenderSystem();
		~InstancedRenderSystem();

		//Called at the start of the draw cycle
		//does nothing in this render system
		void startDraw(GL3DProgram * shader);

		//Add a voxel and maybe draw
		//Called per voxel
		void pushVoxel(GL3DProgram * shader, vec3 pos,int materialId);

		//Called at the end of the draw cycle
		//draws any undrawn voxels
		void finishDraw(GL3DProgram * shader);
	};
	typedef InstancedRenderSystem RenderSystem;
#endif


	RenderSystem renderer;
public:
	VoxelSystem();
	~VoxelSystem();

	//Attempt to load a tile from disc
	bool LoadTile(string tileName);

	//Get the number of voxels drawn last frame
	int GetLastVoxelCount();

	//Draw the voxels in a region
	//atx,aty and tox, toy define two corners of the rectangle
	//of voxels which will be rendered
	void Draw(GL3DProgram * shader, vec3 drawPos, int atx, int aty, int tox, int toy);

	//Get map width
	int GetWidth();
	//Get map height
	int GetHeight();
};