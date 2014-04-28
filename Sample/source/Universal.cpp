#include "stdafx.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "PhysicsSystem.h"
#include "VoxelSystem.h"
#include "PhysicsVoxel.h"

//Traces from rayStart along rayDirection. returns true if ray hits anything
//any parameter can be NULL to indicate no information is desired along that pathway
bool Universal::Trace(vec3 rayStart, vec3 rayDirection, vec3 * rayHitLocation, PhysicsActor ** actorHit, GameMaterial * hitMaterial, vec3 * surfaceNormal) {
	return TraceIgnoreActor(rayStart,rayDirection,NULL,rayHitLocation,actorHit,hitMaterial,surfaceNormal);
}
//Traces from rayStart along rayDirection. returns true if ray hits anything
//Ignores the actor passed to actorToIgnore
//any parameter can be NULL to indicate no information is desired along that pathway
bool Universal::TraceIgnoreActor(vec3 rayStart, vec3 rayDirection, PhysicsActor * actorToIgnore, vec3 * rayHitLocation, PhysicsActor ** actorHit, GameMaterial * hitMaterial, vec3 * surfaceNormal) {
	//Do each trace with separate trace data
	traceData winningTrace;
	winningTrace.traceDistance = 100000;
	traceData currentTrace;
	bool rayHit = false;

	//Trace to actors
	if (Game()->Physics.RaytraceToActor(rayStart,rayDirection,actorToIgnore,currentTrace.traceDistance,currentTrace.surfaceNormal,currentTrace.actorHit)) {
		if (currentTrace.traceDistance < winningTrace.traceDistance) {
			winningTrace = currentTrace;
			winningTrace.hitMaterial = MAT_ACTOR;
			rayHit = true;
		}
	}
	
	//Trace to physics voxels
	if (Game()->Physics.RaytraceToPhysicsVoxel(rayStart,rayDirection,currentTrace.traceDistance,currentTrace.surfaceNormal)) {
		if (currentTrace.traceDistance < winningTrace.traceDistance) {
			winningTrace = currentTrace;
			winningTrace.actorHit = NULL;
			winningTrace.hitMaterial = MAT_PHYSICSVOXEL;
			rayHit = true;
		}
	}

	//Trace to terrain
	if (Game()->Voxels.RaytraceToTerrain(rayStart,rayDirection,currentTrace.traceDistance,currentTrace.surfaceNormal)) {
		if (currentTrace.traceDistance < winningTrace.traceDistance) {
			winningTrace = currentTrace;
			winningTrace.actorHit = NULL;
			winningTrace.hitMaterial = MAT_TERRAIN;
			rayHit = true;
		}
	}


	//Trace to buildings
	//pending
	/*
	if (Game()->Voxels.Raytrace(rayStart,rayDirection,currentTrace.traceDistance,currentTrace.surfaceNormal)) {
		if (currentTrace.traceDistance < winningTrace.traceDistance) {
			winningTrace = currentTrace;
			winningTrace.actorHit = NULL;
			winningTrace.hitMaterial = MAT_STRUCTURE;
		}
	}
	*/

	//Apply data from the winning trace if there was one
	if (rayHit) {
		if (rayHitLocation)
			*rayHitLocation = winningTrace.traceDistance*rayDirection + rayStart;
		if (actorHit)
			*actorHit = winningTrace.actorHit;
		if (hitMaterial)
			*hitMaterial = winningTrace.hitMaterial;
		if (surfaceNormal)
			*surfaceNormal = winningTrace.surfaceNormal;

		return true;
	}
	return false;
}

//Creates a concussion
//damaging terrain
//throwing back physics voxels and actors
void Universal::Concuss(vec3 at, float radius, float damage, PhysicsActor * damagingActor) {
	_ASSERTE(damagingActor != NULL);
	static const int maxVoxelsCreated = 50;
	static const int maxParticlesCreated = 120;
	static const float initialEnergy = 20.0f;
	static const float initialDisplacement = 0.5f;
	//Damage the terrain
	vector<vec4> newPhysicsVoxels = Game()->Voxels.Crater(at,radius*2.0f,damage);
	//Damage the existing physics voxels
	Game()->Physics.Explode(at,radius,damage);
	//Now create new ones based off of the damaged terrain
	float chanceVoxelInstantlyDisintegrates = 1.0f - (float)maxVoxelsCreated/(float)min(maxParticlesCreated,(int)newPhysicsVoxels.size());
	float chanceVoxelInstantlyDissapears = 1.0f - (float)maxParticlesCreated/(float)newPhysicsVoxels.size();
	chanceVoxelInstantlyDisintegrates = max(chanceVoxelInstantlyDisintegrates,0.0f);
	chanceVoxelInstantlyDissapears = max(chanceVoxelInstantlyDissapears,0.0f);

	for (auto newVoxel : newPhysicsVoxels) {
		if (Utilities::random(0.0f,1.0f) <= chanceVoxelInstantlyDissapears)
			continue;

		//Build the voxel
		vec3 voxPos = vec3(newVoxel);
		//Add some random displacement
		voxPos += vec3(Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement));
		//voxel is alive for a random amount of time
		//cheating now, effect voxels are not physics enabled
		PhysicsVoxel * vox = Game()->Physics.BuildVoxel(vec3(newVoxel),Utilities::random(4.0,6.0));
		//set material and a random velocity
		vox->MaterialId = (int)newVoxel.w;
		vox->Velocity = vec3(Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy));

		//If its to instantly disintegrate, change it's deathat
		if (Utilities::random(0.0f,1.0f) >= (1-chanceVoxelInstantlyDisintegrates)) 
			vox->DeathAt = 1.0;
	}
	//Do aoe damage to actors
	Game()->Actors.DoAOEDamage(at,radius,damage,damagingActor);
}