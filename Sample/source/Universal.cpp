#include "stdafx.h"
#include "Universal.h"
#include "BaseFrame.h"
#include "PhysicsSystem.h"
#include "VoxelSystem.h"

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