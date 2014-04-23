
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


	//Collide physics voxels with other physics voxels
	void collideVoxelsToVoxels();
	//Collide physics voxels with actors
	void collideVoxelsToActors();
	//Collide actors with actors
	void collideActorsToActors();


	//Update and finalize physics voxels
	//also handles terrain interactions
	void updatePhysicsVoxels();
	//Update and finalize physics actors
	//also handles terrain interactions
	void updatePhysicsActors();

	//A tie to the voxel system used to lookup the terrain height at various points
	VoxelSystem * voxelSystem;

public:
	PhysicsSystem(VoxelSystem * system);
	~PhysicsSystem();

	//Traces a line to the first intersecting physics voxel
	bool RaytraceToPhysicsVoxel(vec3 from, vec3 direction, float & rayLength, vec3 & surfaceNormal);

	//Traces a line to the first intersecting physics actor
	bool RaytraceToActor(vec3 from, vec3 direction, PhysicsActor * toIgnore, float & rayLength, vec3 & surfaceNormal, PhysicsActor * & hitActor);

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate, double lifeTime=-1.0);

	//Used by PhysicsActor to register/unregister itself. Not needed elsewhere
	void RegisterPhysicsActor(PhysicsActor * toRegister);
	void UnregisterPhysicsActor(PhysicsActor * toUnregister);

	//Create an explosion affecting physics voxels and forcing them back
	void Explode(vec3 pos, float radius, float damage);

	//Update the physics voxels, called by base frame
	void Update();

	//Draw all the actors
	void Draw(ShaderGroup * shaders);

	//Get a Pointer to Actors
	ContiguousList<PhysicsActor*>* GetActors();

	//Called right before a physics voxel is destroyed
	GameEvent<void(PhysicsVoxel*)> VoxelDisintegrating;

	//Physics save data
	CLASS_DECLARATION(PhysicsSystem)
		CLASS_CONTAINER_MEMBER(allVoxels,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_CONTAINER_MEMBER(actors,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};