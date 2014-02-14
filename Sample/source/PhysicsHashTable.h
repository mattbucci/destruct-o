
#pragma once

#include "stdafx.h"

class PhysicsVoxel;

//100 buckets is more than enough, in fact 50 might be better
//but we'll do 100 for now
#define PHYSICS_HASH_TABLE_BUCKETS 100

//This hash table uses the spatial orientation of a physicsvoxel
//and places it in buckets of things that could collide
//Currently only uses the X axis
//Note: each voxel may be in several locations in the hashtable
class PhysicsHashTable {
	//Spatial buckets
	//each bucket is an array of PhysicsVoxel*
	PhysicsVoxel ** buckets[PHYSICS_HASH_TABLE_BUCKETS];
	//count of items in bucket
	int bucketSize[PHYSICS_HASH_TABLE_BUCKETS];
	//count of capacity of bucket
	int bucketCapacity[PHYSICS_HASH_TABLE_BUCKETS];

	//Push an item to a bucket
	//right now buckets can grow but not shrink
	//this should be fixed eventually
	void PushToBucket(int bucket, PhysicsVoxel * toPush);
public:
	PhysicsHashTable();
	~PhysicsHashTable();

	//Empty the table
	void Clear();

	//Add the particular voxel to location(s) in the table
	void Add(PhysicsVoxel * voxel);

	//Get the array of voxels stored in that particular bucket
	PhysicsVoxel ** operator[](const int bucket);

	//Get the size of the specified bucket
	int Size(const int bucket);
};