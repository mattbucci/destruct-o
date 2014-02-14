#include "stdafx.h"

struct face {
	vec3 normal;
	vec3 point;
};

struct VoxelHull {
	vec3 vertices[8];
	face faces[6];
};

class PhysicsVoxel {
public:
	PhysicsVoxel() {}
	//Almost all properties are public for speed

	bool AabColiding(vec3 otherBox);


	bool colliding;

	//Up to date hull being used by this voxel
	VoxelHull Hull;


	//Acceleration calculated every frame
	vec3 Acceleration;

	//Velocity (momentum) of the voxel
	vec3 Velocity;
	//The position of the center of mass
	//the draw position is -.5,-.5 from this position (I think?)
	vec3 Position;
};