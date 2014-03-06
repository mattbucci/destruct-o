
#pragma once

#include "stdafx.h"

class PhysicsTriangle {
public:
	PhysicsTriangle(vec3 a, vec3 b, vec3 c);
	vec3 a;
	vec3 b;
	vec3 c;
	vec3 normal;

	static bool RayIntersects(PhysicsTriangle t, vec3 trianglePosition, const vec3 & startPoint,const vec3 & direction,double * intersectionDistance, vec3 * surfaceNormal);
};