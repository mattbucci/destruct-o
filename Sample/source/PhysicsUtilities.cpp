#include "stdafx.h"
#include "PhysicsUtilities.h"
#include "PhysicsTriangle.h"


float PhysicsUtilities::removeInDirection(vec3 & force, vec3 direction) {
	float d = glm::dot(force,direction);
	if (d > 0) {
		force -= direction*d;
		return d;
	}
	return 0.0f;
}


//Given two voxel positions find intersection information
Intersection PhysicsUtilities::CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition) {
	Intersection data;

	if (voxelAPosition == voxelBPosition)
		voxelAPosition += .0001;

	//Estimate penetration depth as the greatest difference between A and B
	//this works for cubes only
	vec3 collisionVector = voxelAPosition-voxelBPosition;
	vec3 difference = glm::abs(collisionVector);
	//voxels are 1 large and axis aligned so this works
	data.Depth = 1.0f-max(difference.x,max(difference.y,difference.z));
	data.CollisionVector = glm::normalize(collisionVector);


	//Estimate normal as the closest normal to the collision vector
	if ((difference.x > difference.y) && (difference.x > difference.z)) {
		//x is largest
		if (collisionVector.x > 0)
			data.Normal = vec3(1,0,0);
		else
			data.Normal = vec3(-1,0,0);
	}
	else if (difference.y > difference.z) {
		//y is largest
		if (collisionVector.y > 0)
			data.Normal = vec3(0,1,0);
		else
			data.Normal = vec3(0,-1,0);
	}
	else {
		//x is largest
		if (collisionVector.z > 0)
			data.Normal = vec3(0,0,1);
		else
			data.Normal = vec3(0,0,-1);
	}

	return data;
}
//Given a voxel position and an extents box find intersection data
Intersection PhysicsUtilities::CalculateIntersection(vec3 voxelAPosition, vec3 cuboidPosition, vec3 halfCuboidSize) {
	Intersection data;

	if (voxelAPosition == cuboidPosition)
		cuboidPosition += .0001;

	//Estimate penetration depth as the greatest difference between A and B
	//this works for cubes only
	vec3 collisionVector = cuboidPosition-voxelAPosition;
	vec3 difference = glm::abs(collisionVector);
	//Now subtract half the cuboid size to get interpenetration sizes
	difference = (halfCuboidSize+.5f)-difference;
	//find the penetration depth as the smallest possible penetration
	data.Depth = min(difference.x,min(difference.y,difference.z));
	data.CollisionVector = glm::normalize(collisionVector);


	//Estimate normal as the closest normal to the collision vector
	if ((difference.x < difference.y) && (difference.x < difference.z)) {
		//x is smallest
		if (collisionVector.x > 0)
			data.Normal = vec3(1,0,0);
		else
			data.Normal = vec3(-1,0,0);
	}
	else if (difference.y < difference.z) {
		//y is smallest
		if (collisionVector.y > 0)
			data.Normal = vec3(0,1,0);
		else
			data.Normal = vec3(0,-1,0);
	}
	else {
		//z is smallest
		if (collisionVector.z > 0)
			data.Normal = vec3(0,0,1);
		else
			data.Normal = vec3(0,0,-1);
	}

	return data;
}

//Given two extents boxes find the intersection
Intersection PhysicsUtilities::CalculateIntersection(vec3 cuboidPositionA, vec3 halfCuboidSizeA, vec3 cuboidPositionB, vec3 halfCuboidSizeB) {
	Intersection data;

	if (cuboidPositionA == cuboidPositionB)
		cuboidPositionB += .0001;

	//Estimate penetration depth as the greatest difference between A and B
	//this works for cubes only
	vec3 collisionVector = cuboidPositionB-cuboidPositionA;
	vec3 difference = glm::abs(collisionVector);
	//Now subtract half the cuboid size to get interpenetration sizes
	difference = (halfCuboidSizeA+halfCuboidSizeB)-difference;
	//find the penetration depth as the smallest possible penetration
	data.Depth = min(difference.x,min(difference.y,difference.z));
	data.CollisionVector = glm::normalize(collisionVector);


	//Estimate normal as the closest normal to the collision vector
	if ((difference.x < difference.y) && (difference.x < difference.z)) {
		//x is smallest
		if (collisionVector.x > 0)
			data.Normal = vec3(1,0,0);
		else
			data.Normal = vec3(-1,0,0);
	}
	else if (difference.y < difference.z) {
		//y is smallest
		if (collisionVector.y > 0)
			data.Normal = vec3(0,1,0);
		else
			data.Normal = vec3(0,-1,0);
	}
	else {
		//z is smallest
		if (collisionVector.z > 0)
			data.Normal = vec3(0,0,1);
		else
			data.Normal = vec3(0,0,-1);
	}

	return data;
}



bool PhysicsUtilities::TraceToVoxel(const vec3 & from, const vec3 & direction, vec3 at, float & rayLength, vec3 & surfaceNormal) {
	static PhysicsTriangle voxelTriangles[12] = {
		//Top
		PhysicsTriangle(vec3(0,0,1), vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(0,0,1), vec3(1.0f,1.0f,1.0f),
		vec3(0.0f,1.0f,1.0f),
		vec3(1.0f,0.0f,1.0f)),
		//Bottom
		PhysicsTriangle(vec3(0,0,-1), vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f)),
		PhysicsTriangle(vec3(0,0,-1), vec3(1.0f,1.0f,0.0f),
		vec3(0.0f,1.0f,0.0f),
		vec3(1.0f,0.0f,0.0f)),
		//left
		PhysicsTriangle(vec3(-1,0,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(-1,0,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		//right
		PhysicsTriangle(vec3(1,0,0), vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(1,0,0), vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
		//front
		PhysicsTriangle(vec3(0,-1,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f)),
		PhysicsTriangle(vec3(0,-1,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f)),
		//back
		PhysicsTriangle(vec3(0,1,0), vec3( 0.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(0,1,0), vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
	};
	//Check every triangle in this voxel for a collision
	for (int i = 0 ; i < 12; i++) {
		double surf;
		vec3 norm;
		if (PhysicsTriangle::RayIntersects(voxelTriangles[i],at,from,direction,&surf,&norm)) {
			rayLength = (float)surf;
			surfaceNormal = norm;
			//Stop checking voxels
			return true;
		}
	}
	return false;
}



bool PhysicsUtilities::TraceToCuboid(const vec3 & from, const vec3 & direction, vec3 at, vec3 size, float & rayLength, vec3 & surfaceNormal) {
	PhysicsTriangle voxelTriangles[12] = {
		//Top
		PhysicsTriangle(vec3(0,0,1), vec3( 0.0f,0.0f,size.z),
		vec3( size.x,0.0f,size.z),
		vec3( 0.0f,size.y,size.z)),
		PhysicsTriangle(vec3(0,0,1), vec3(size.x,size.y,size.z),
		vec3(0.0f,size.y,size.z),
		vec3(size.x,0.0f,size.z)),
		//Bottom
		PhysicsTriangle(vec3(0,0,-1), vec3( 0.0f,0.0f,0.0f),
		vec3(size.x,0.0f,0.0f),
		vec3( 0.0f,size.y,0.0f)),
		PhysicsTriangle(vec3(0,0,-1), vec3(size.x,size.y,0.0f),
		vec3(0.0f,size.y,0.0f),
		vec3(size.x,0.0f,0.0f)),
		//left
		PhysicsTriangle(vec3(-1,0,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,size.y,0.0f),
		vec3( 0.0f,size.y,size.z)),
		PhysicsTriangle(vec3(-1,0,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,size.z),
		vec3( 0.0f,size.y,size.z)),
		//right
		PhysicsTriangle(vec3(1,0,0), vec3( size.x,0.0f,0.0f),
		vec3( size.x,size.y,0.0f),
		vec3( size.x,size.y,size.z)),
		PhysicsTriangle(vec3(1,0,0), vec3( size.x,0.0f,0.0f),
		vec3( size.x,0.0f,size.z),
		vec3( size.x,size.y,size.z)),
		//front
		PhysicsTriangle(vec3(0,-1,0), vec3( 0.0f,0.0f,0.0f),
		vec3( size.x,0.0f,0.0f),
		vec3( size.x,0.0f,size.z)),
		PhysicsTriangle(vec3(0,-1,0), vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,size.z),
		vec3( size.x,0.0f,size.z)),
		//back
		PhysicsTriangle(vec3(0,1,0), vec3( 0.0f,size.y,0.0f),
		vec3( size.x,size.y,0.0f),
		vec3( size.x,size.y,size.z)),
		PhysicsTriangle(vec3(0,1,0), vec3( 0.0f,size.y,0.0f),
		vec3( 0.0f,size.y,size.z),
		vec3( size.x,size.y,size.z)),
	};
	//Check every triangle in this cuboid for a collision
	for (int i = 0 ; i < 12; i++) {
		double surf;
		vec3 norm;
		if (PhysicsTriangle::RayIntersects(voxelTriangles[i],at,from,direction,&surf,&norm)) {
			rayLength = (float)surf;
			surfaceNormal = norm;
			//Stop checking voxels
			return true;
		}
	}
	return false;
}

