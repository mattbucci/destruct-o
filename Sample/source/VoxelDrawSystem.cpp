#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "BasicVoxelRenderSystem.h"
#include "InstancedVoxelRenderSystem.h"

VoxelDrawSystem::~VoxelDrawSystem() {
	//Virtual, so body required
}


VoxelDrawSystem * VoxelDrawSystem::BuildAppropriateSystem()
{
	//Choose the appropriate render version
	switch (OpenglVersion){
	case 20:
		return new BasicVoxelRenderSystem();
		break;
	case 31:
		return new InstancedVoxelRenderSystem();
		break;
	}
    
    // Function needs to have a definitive return.
    _ASSERTE(false);
    return NULL;
}
