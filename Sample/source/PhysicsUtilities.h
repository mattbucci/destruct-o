
#pragma once

//Once an intersection is found, calculate this information about it
struct Intersection {
	//The normal of the plane being intersected
	vec3 Normal;
	//The (approx) depth of the penetration
	float Depth;
	//The direction vector from B to A
	vec3 CollisionVector;
};

class PhysicsUtilities {
public:

	//C style function for performance reasons
	//Given two voxel positions find intersection information
	static Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
	static Intersection CalculateIntersection(vec3 voxelAPosition, vec3 cuboidPosition, vec3 halfCuboidSize);
	static Intersection CalculateIntersection(vec3 cuboidPositionA, vec3 halfCuboidSizeA, vec3 cuboidPositionB, vec3 halfCuboidSizeB);

	//Remove the force in a given direction and return the quantity
	static float removeInDirection(vec3 & force, vec3 direction);

	//Check if the given ray intersecs with the given voxel
	//if it does return true and set intersection position and surface normal
	//otherwise return true
	static bool TraceToVoxel(const vec3 & from, const vec3 & direction, vec3 at, float & rayLength, vec3 & surfaceNormal);
	static bool TraceToCuboid(const vec3 & from, const vec3 & direction, vec3 at, vec3 size, float & rayLength, vec3 & surfaceNormal);

	//A generic truncate function
	template<class T>
	static T _trunc(T d){ return (d>0) ? floor(d) : ceil(d) ; }
};