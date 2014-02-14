#include "stdafx.h"
#include "PhysicsHashTable.h"
#include "PhysicsVoxel.h"


PhysicsHashTable::PhysicsHashTable() {
	//Start off each bucket with a starting size
	for (int i = 0; i < PHYSICS_HASH_TABLE_BUCKETS; i++) {
		buckets[i] = new PhysicsVoxel*[30];
		bucketSize[i] = 0;
		bucketCapacity[i] = 30;
	}
}
PhysicsHashTable::~PhysicsHashTable() {
	//Cleanup buckets
	for (int i = 0; i < PHYSICS_HASH_TABLE_BUCKETS; i++)
		delete [] buckets;
}

//Empty the table
void PhysicsHashTable::Clear() {
	for (int i = 0; i < PHYSICS_HASH_TABLE_BUCKETS; i++)
		bucketSize[i] = 0;
}

//Add the particular voxel to location(s) in the table
void PhysicsHashTable::Add(PhysicsVoxel * voxel) {
	int firstIndex = (int)voxel->Position.x % PHYSICS_HASH_TABLE_BUCKETS;
	//There are two possible places the voxel could go
	PushToBucket(firstIndex,voxel);
	//Now check if it is spilling over into the adjacent cell
	int secondIndex = (int)ceil(voxel->Position.x) % PHYSICS_HASH_TABLE_BUCKETS;
	if (secondIndex != firstIndex) {
		//If it's a unique index add it too
		PushToBucket(secondIndex,voxel);
	}
}

//Right now buckets can grow but not shrink
void PhysicsHashTable::PushToBucket(int bucket, PhysicsVoxel * toPush) {
	//Reference these things only once
	int & size = bucketSize[bucket];
	int & capacity = bucketCapacity[bucket];
	PhysicsVoxel ** & oldBucket = buckets[bucket];
	//Not enough room for one more, expand the bucket
	if (size == capacity) {
		//Add 10 to the capacity of the new bucket (hopefully enough...)
		PhysicsVoxel ** newBucket = new PhysicsVoxel*[capacity+10];
		memcpy(newBucket,oldBucket,sizeof(PhysicsVoxel**)*capacity);
		delete [] oldBucket;
		oldBucket = newBucket;
		capacity += 10;
	}
	//Push the voxel
	oldBucket[size++] = toPush;
}

//Get the array of voxels stored in that particular bucket
PhysicsVoxel ** PhysicsHashTable::operator[](const int bucket) {
	return buckets[bucket];
}

//Get the size of the specified bucket
int PhysicsHashTable::Size(const int bucket) {
	return bucketSize[bucket];
}