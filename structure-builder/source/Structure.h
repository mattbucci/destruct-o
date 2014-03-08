#pragma once

#include "stdafx.h"
#include "ContiguousList.h"

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

	//A list of each voxel in the structure
	ContiguousList<StructureCell> Cells; 

	bool TraceToStructure(

	Structure();
	~Structure();
};