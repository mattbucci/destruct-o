#pragma once

#include "stdafx.h"
#include "ContiguousList.h"

class GL3DProgram;
class VoxelDrawSystem;

struct StructureCell {
	vec3 pos;
	unsigned int material;
};

class Structure {
	//Write the raw memory in both formats automatically
	template<class T>
	static void Write(T value, vector<unsigned char> & platformEndian, vector<unsigned char> & swappedEndian) {
		unsigned char * byte = (unsigned char*)&value;
		for (unsigned int i = 0; i < sizeof(T); i++)
			platformEndian.push_back(byte[i]);
		for (unsigned int i = sizeof(T)-1; i >= 0; i--)
			swappedEndian.push_back(byte[i]);
	}

	//Read from raw memory
	template<class T>
	static void Read(T & readValue, vector<unsigned char> & platformFile, int & filePos) {
		unsigned char * byte = (unsigned char*)&readValue;
		for (int i = 0; i < sizeof(T); i++)
			byte[i] = platformFile[filePos+i];
		filePos += sizeof(T);
	}

	static bool WriteFile(string name, vector<unsigned char> data);
	static vector<unsigned char> ReadFile(string name);
public:
	//Load a structure or floor out of disc
	static Structure * LoadStructure(string structureName);
	//Save a structure or floor to disc
	void SaveStructure(string structureName);

	//Extents used by the editor and the game
	vec3 Extents;

	//A list of each voxel in the structure
	vector<StructureCell> Cells; 

	//Attempt to ray trace to this structure
	bool Trace(const vec3 & from, const vec3 & direction, vec3 & rayCollision, vec3 & surfaceNormal);

	//Ray trace to a specific voxel position instead of intersection
	bool EditorTraceToVoxel(const vec3 & from, const vec3 & direction, vec3 & voxelCollidedWith, vec3 & surfaceNormal);

	//Remove voxel at position
	void EditorRemoveVoxel(vec3 position);

	//Add voxel at position. If a voxel already exists in that position do nothing
	void EditorAddVoxel(vec3 position, int materialId);

	//Render all voxels in this structure in a single pass
	void EditorRenderStructure(GL3DProgram * shader, VoxelDrawSystem * drawSystem);
};