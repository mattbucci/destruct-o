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