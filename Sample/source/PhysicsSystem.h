
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "ContiguousList.h"

class ShaderGroup;
class PhysicsVoxel;
//The voxel system is needed to lookup the height of the terrain
class VoxelSystem;
class PhysicsProxy;
class Actor;
class ParticleData;

//The true physics system
class PhysicsSystem : public Savable {
	//Voxel list now uses a contiguous list for speed
	//designed for fast insert/removal
	//stored objects are always contiguous
	ContiguousList<PhysicsVoxel*> allVoxels;
	
	//this contiguous list is a sub buffer used to severely decrease the AABB checks needed for physics voxels
	ContiguousList<PhysicsVoxel*> section;

	//A list of all actor proxies the physics system owns
	ContiguousList<PhysicsProxy*> proxies;


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

	//C style function for performance reasons
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);

	//Temporary particle data
	ParticleData * physicsVoxelErase;
public:
	PhysicsSystem(VoxelSystem * system);
	~PhysicsSystem();

	//Traces a line to the first intersecting terrain or physics voxel
	bool Raytrace(vec3 from, vec3 direction, vec3 & rayCollision, vec3 & surfaceNormal);

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate, double lifeTime=-1.0);

	//Construct a physics proxy for the given actor
	//once attached, detach() must be called on actor destruction
	PhysicsProxy * BuildPhysicsProxy(Actor * actor, vec3 * actorPosition);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);


	CLASS_DECLARATION(PhysicsSystem)
		CLASS_CONTAINER_MEMBER(allVoxels,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};