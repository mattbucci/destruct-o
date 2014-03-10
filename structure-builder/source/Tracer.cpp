#include "stdafx.h"
#include "Tracer.h"
#include "PhysicsTriangle.h"


bool Tracer::TraceToVoxel(const vec3 & from, const vec3 & direction, vec3 at, vec3 & rayCollision, vec3 & surfaceNormal) {
	static PhysicsTriangle voxelTriangles[12] = {
		//Top
		PhysicsTriangle(vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(1.0f,1.0f,1.0f),
		vec3(-1.0f,1.0f,1.0f),
		vec3(1.0f,0.0f,1.0f)),
		//Bottom
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f)),
		PhysicsTriangle(vec3(1.0f,1.0f,0.0f),
		vec3(-1.0f,1.0f,0.0f),
		vec3(1.0f,0.0f,0.0f)),
		//left
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		//right
		PhysicsTriangle(vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
		//front
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f)),
		//back
		PhysicsTriangle(vec3( 0.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
	};
	//Check every triangle in this voxel for a collision
	for (int i = 0 ; i < 12; i++) {
		double surf;
		vec3 norm;
		if (PhysicsTriangle::RayIntersects(voxelTriangles[i],at,from,direction,&surf,&norm)) {
			rayCollision = ((float)surf)*direction+from;
			surfaceNormal = norm;
			//Stop checking voxels
			return true;
		}
	}
	return false;
}

