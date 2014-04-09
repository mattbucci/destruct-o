#include "stdafx.h"
#include "PhysicsTriangle.h"

PhysicsTriangle::PhysicsTriangle(vec3 normal, vec3 a, vec3 b, vec3 c) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->normal = normal;
}

bool PhysicsTriangle::RayIntersects(PhysicsTriangle t, vec3 trianglePosition, const vec3 & startPoint,const vec3 & direction,double * intersectionDistance, vec3 * surfaceNormal) {
	double epsilon = .00001;

	//Dont check back face for collisions
	if (glm::dot(t.normal,direction) > 0)
		return false;

	//Move the triangle to the given position
	t.a += trianglePosition;
	t.b += trianglePosition;
	t.c += trianglePosition;

	vec3 e1 = t.b-t.a;
	vec3 e2 = t.c-t.a;
	vec3 q = glm::cross(direction,e2);
	double ah = glm::dot(e1,q);

	//vec3 is parallel to plane
	if ((ah>-epsilon) && (ah<epsilon))
		return false;

	double f = 1/ah;
	vec3 s = startPoint-t.a;
	double u = f*glm::dot(s,q);

	//Intersection outside of triangle
	if (u < 0)
		return false;

	vec3 r = glm::cross(s,e1);
	double v = f*glm::dot(direction,r);

	//Intersection outside of triangle
	if ((v < 0.0) || ((u+v)>1.0))
		return false;

	*intersectionDistance = f*glm::dot(e2,r);
	//*intersectionPoint = startPoint+direction*t;
	*surfaceNormal = t.normal;

	if (*intersectionDistance < 0)
		return false; //ray went wrong direction

	return true;
}