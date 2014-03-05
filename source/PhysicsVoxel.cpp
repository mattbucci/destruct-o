#include "stdafx.h"
#include "PhysicsVoxel.h"

bool PhysicsVoxel::AabColiding(vec3 otherBox) {
	vec3 diff = glm::abs(Position-otherBox);
	return ((diff.x < 1.0f) &&
		(diff.y < 1.0f) &&
		(diff.z < 1.0f));
}

