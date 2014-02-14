
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"

class ShaderGroup;
class PhysicsVoxel;
//The voxel system is needed to lookup the height of the terrain
class VoxelSystem;

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

	//Once an intersection is found, calculate this information about it
	struct Intersection {
		//The normal of the plane being intersected
		vec3 Normal;
		//The (approx) depth of the penetration
		float Depth;
		//The direction vector from B to A
		vec3 CollisionVector;
	};

	//A tie to the voxel system used to lookup the terrain height at various points
	VoxelSystem * voxelSystem;



	//C style function for performance reasons
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
public:
	PhysicsSystem(VoxelSystem * system);
	~PhysicsSystem();

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);
};