#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "BasicVoxelRenderSystem.h"
#include "InstancedVoxelRenderSystem.h"

VoxelDrawSystem::~VoxelDrawSystem() {
	//Virtual, so body required
}


VoxelDrawSystem * VoxelDrawSystem::BuildAppropriateSystem()
{
#ifdef __MOBILE__
    // iOS will ALWAYS use the instanced render system
    return new InstancedVoxelRenderSystem();
#else
	//Choose the appropriate render version
	switch (OpenglVersion){
	case 20:
		return new BasicVoxelRenderSystem();
		break;
//Android can't use instanced rendering so its not even an option
	case 31:
		return new InstancedVoxelRenderSystem();
		break;
	}
#endif
    
    // Function needs to have a definitive return.
    _ASSERTE(false);
    return NULL;
}
