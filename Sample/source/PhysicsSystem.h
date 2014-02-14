
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "PhysicsHashTable.h"

class ShaderGroup;
class PhysicsVoxel;


#include "stdafx.h"

//The true physics system
class PhysicsSystem {
	//All actors are updated and drawn
	vector<PhysicsVoxel*> allVoxels;
	//the lowest empty slot in the allActors array
	unsigned int lastEmpty;
	//the highest full slot in the allActors array
	unsigned int highestFull;

	//The voxel draw renderer
	VoxelDrawSystem * renderer;

	//Physics Voxel
	void Register(PhysicsVoxel * toRegister);
	void Unregister(PhysicsVoxel * toUnregister);

	//Tracks voxels placing them in buckets in which a voxel
	//can only ever possibly collide with other voxels in the same bucket
	//though a voxel may be in multiple buckets
	PhysicsHashTable voxelCollisionTable;

	//Once an intersection is found, calculate this information about it
	struct Intersection {
		//The normal of the plane being intersected
		vec3 Normal;
		//The (approx) depth of the penetration
		float Depth;
		//The direction vector from B to A
		vec3 CollisionVector;
	};

	//C style function for performance reasons
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
public:
	PhysicsSystem();
	~PhysicsSystem();

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);
};