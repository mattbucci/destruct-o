#pragma once

#include "stdafx.h"
#include "ContiguousList.h"

struct StructureCell {
	vec3 pos;
	unsigned int material;
};

class Structure {
public:
	
	ContiguousList<StructureCell> Cells; 

	Structure();
	~Structure();
};