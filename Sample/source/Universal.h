
#pragma once

#include "stdafx.h"

class PhysicsActor;

class Universal {
public:
	enum GameMaterial {
		MAT_NOTHING = 0,
		MAT_TERRAIN,
		MAT_ACTOR,
		MAT_PHYSICSVOXEL,
		MAT_STRUCTURE,
	};
private:
	struct traceData {
		float traceDistance;
		PhysicsActor * actorHit;
		GameMaterial hitMaterial;
		vec3 surfaceNormal;
	};
public:
	//This struct will be used for advanced traces
	//in the future
	/*
	//This struct is not savable and thus should never
	//be stored long-term
	struct TraceOptions {
		TraceOptions();
		//The actor to ignore
		PhysicsActor * actorToIgnore;
		//the min distance the trace can travel before giving up
		//the trace may go for much longer than this
		//or be much shorter than this if it hits something
		int minTraceDistance;
		//unused - may be used to color traces
		//for debug purposes
		vec4 debugTraceColor;
	}*/

	//Creates a concussion
	//damaging terrain
	//throwing back physics voxels and actors
	static void Concuss(vec3 at, float radius, float damage, PhysicsActor * damagingActor);


	//Traces from rayStart along rayDirection. returns true if ray hits anything
	//any parameter can be NULL to indicate no information is desired along that pathway
	static bool Trace(vec3 rayStart, vec3 rayDirection, vec3 * rayHitLocation, PhysicsActor ** actorHit = NULL, GameMaterial * hitMaterial = NULL, vec3 * surfaceNormal = NULL);
	//Traces from rayStart along rayDirection. returns true if ray hits anything
	//Ignores the actor passed to actorToIgnore
	//any parameter can be NULL to indicate no information is desired along that pathway
	static bool TraceIgnoreActor(vec3 rayStart, vec3 rayDirection, PhysicsActor * actorToIgnore, vec3 * rayHitLocation, PhysicsActor ** actorHit = NULL, GameMaterial * hitMaterial = NULL, vec3 * surfaceNormal = NULL);
};