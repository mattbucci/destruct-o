
#pragma once

class Tracer {
public:
	//Check if the given ray intersecs with the given voxel
	//if it does return true and set intersection position and surface normal
	//otherwise return true
	static bool TraceToVoxel(const vec3 & from, const vec3 & direction, vec3 at, vec3 & rayCollision, vec3 & surfaceNormal);
};