
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "ContiguousList.h"
#include "GameEvent.h"

class ShaderGroup;
class PhysicsVoxel;
//The voxel system is needed to lookup the height of the terrain
class VoxelSystem;
class PhysicsActor;
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

	//A list of physics actors
	ContiguousList<PhysicsActor*> actors;

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


	//Collide physics voxels with other physics voxels
	void collideVoxelsToVoxels(float delta);
	//Collide physics voxels with actors
	void collideVoxelsToActors(float delta);
	//Collide actors with actors
	void collideActorsToActors(float delta);


	//Update and finalize physics voxels
	//also handles terrain interactions
	void updatePhysicsVoxels(float delta);
	//Update and finalize physics actors
	//also handles terrain interactions
	void updatePhysicsActors(float delta);

	//A tie to the voxel system used to lookup the terrain height at various points
	VoxelSystem * voxelSystem;

	//C style function for performance reasons
	//Given two voxel positions find intersection information
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 cuboidPosition, vec3 halfCuboidSize);
	friend Intersection CalculateIntersection(vec3 cuboidPositionA, vec3 halfCuboidSizeA, vec3 cuboidPositionB, vec3 halfCuboidSizeB);

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

	//Used by PhysicsActor to register/unregister itself. Not needed elsewhere
	void RegisterPhysicsActor(PhysicsActor * toRegister);
	void UnregisterPhysicsActor(PhysicsActor * toUnregister);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);

	//Called right before a physics voxel is destroyed
	GameEvent<void(PhysicsVoxel*)> VoxelDisintegrating;
	CLASS_DECLARATION(PhysicsSystem)
		CLASS_CONTAINER_MEMBER(allVoxels,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};