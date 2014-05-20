#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "InstancedVoxelRenderSystem.h"

VoxelDrawSystem::~VoxelDrawSystem() {
	//Virtual, so body required
}


VoxelDrawSystem * VoxelDrawSystem::BuildAppropriateSystem()
{
	return new InstancedVoxelRenderSystem();
}
