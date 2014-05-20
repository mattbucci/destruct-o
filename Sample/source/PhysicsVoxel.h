#include "stdafx.h"

class PhysicsVoxel : public Savable{
public:
	//Almost all properties are public for speed

	//Assuming this voxel is axis aligned check if it is colliding
	//with another voxel whos center of mass is	'otherBox'
	bool AabColiding(vec3 otherBox);

	//Acceleration calculated every frame
	vec3 Acceleration;

	//Velocity (momentum) of the voxel
	vec3 Velocity;

	//The position of the center of mass
	//the draw position is -.5,-.5 from this position 
	vec3 Position;

	//Voxel will use this material
	int MaterialId;

	//If the current time is greater than this the voxel is disintegrated
	//if<0 voxel lives forever
	double DeathAt;

	//if the voxel can collide with other objects
	bool cancollide;

	CLASS_DECLARATION(PhysicsVoxel)
		CLASS_MEMBER(Acceleration,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(Velocity,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(Position,ReflectionData::SAVE_VEC3)
		CLASS_MEMBER(MaterialId,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(DeathAt,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(cancollide, ReflectionData::SAVE_BOOL)
	END_DECLARATION
};