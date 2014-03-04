
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "ContiguousList.h"

class ShaderGroup;
class PhysicsVoxel;
//The voxel system is needed to lookup the height of the terrain
class VoxelSystem;

#include "stdafx.h"

//The true physics system
class PhysicsSystem {
	//Voxel list now uses a contiguous list for speed
	//designed for fast insert/removal
	//stored objects are always contiguous
	ContiguousList<PhysicsVoxel> allVoxels;

	//The voxel draw renderer
	VoxelDrawSystem * renderer;

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

	//3d ray trace to a single voxel placed at "at"
	bool checkForCollision(const vec3 & from, const vec3 & direction, vec3 at, vec3 & rayCollision, vec3 & surfaceNormal) ;

	//C style function for performance reasons
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
public:
	PhysicsSystem(VoxelSystem * system);
	~PhysicsSystem();

	//Traces a line to the first intersecting terrain or physics voxel
	bool Raytrace(vec3 from, vec3 direction, vec3 & rayCollision, vec3 & surfaceNormal);

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate, double lifeTime=-1.0);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);
};