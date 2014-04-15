
#pragma once

#include "stdafx.h"
#include "VoxelDrawSystem.h"
#include "ContiguousList.h"

class ShaderGroup;
class PhysicsVoxel;
//The voxel system is needed to lookup the height of the terrain
class VoxelSystem;
class PhysicsActor;
class Actor;

//The true physics system
class PhysicsSystem : public Savable {
	//Voxel list now uses a contiguous list for speed
	//designed for fast insert/removal
	//stored objects are always contiguous
	ContiguousList<PhysicsVoxel*> allVoxels;
	
	//this contiguous list is a sub buffer used to severely decrease the AABB checks needed for physics voxels
	ContiguousList<PhysicsVoxel*> section;

	//A list of physics actors
	ContiguousList<PhysicsActor*> actors;

	//The voxel draw renderer
	VoxelDrawSystem * renderer;

	//Once an intersection is found, calculate this information about it
	struct Intersection {
		//The normal of the plane being intersected
		vec3 Normal;
		//The (approx) depth of the penetration
		float Depth;
		//The direction vector from B to A
		vec3 CollisionVector;
	};

	//A generalized update function
	//which can be applied to voxels and physics actors
	template <class T>
	void updatePhysicsSystem(ContiguousList<T> & physicsItems, float delta) {
		//Update all the physics voxels
		for (auto voxel : physicsItems) {
 			voxel->Acceleration += vec3(0,0,-10);
	
			//Do ground collisions now
			//There are (usually) four different tiles you're over
			vec2 floorTiles[4];
			floorTiles[0] = glm::floor(vec2(voxel->Position));
			floorTiles[1] = vec2(floor(voxel->Position.x),ceil(voxel->Position.y));
			floorTiles[2] = vec2(ceil(voxel->Position.x),floor(voxel->Position.y));
			floorTiles[3] = vec2(ceil(voxel->Position.x),ceil(voxel->Position.y));
			//If the voxel is fully below the surface of each floor tile checked, than it shall be teleported to the lowest surface
			int depthTilesViolated = 0;
			float lowestHeight = 300;
			//Use these four blocks as a support for any blocks on top of the ground
			for (int i = 0; i < 4; i++) {
				//check if the square is under the ground for this ground tile
				float height = voxelSystem->GetPositionHeight(floorTiles[i]);
				if ((height+.5) < voxel->Position.z)
					continue;

				//Check if the depth constraint of this tile is being violated
				if ((height-2.5) > voxel->Position.z) {
					depthTilesViolated++;
					lowestHeight = min(lowestHeight,height);
				}
				
				//So the block must be penetrating this block of terrain
				//time to reject it
				//simulate a block next to the penetrating voxel
				Intersection intersectionData = CalculateIntersection(voxel->Position,vec3(floorTiles[i],min(height-.5f,voxel->Position.z))+vec3(.5,.5,0));
				//If the depth is < 0 then you're not penetrating quite yet...
				if (intersectionData.Depth < 0)
					continue;

				float force = 500*intersectionData.Depth;

				vec3 forceDirection = intersectionData.Normal;
				voxel->Acceleration += forceDirection*force;
				//Remove Velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(voxel->Velocity,-forceDirection);
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 3.0f;
				voxel->Velocity += vel*forceDirection;
				//Apply extra enhanced friction while they're touching
				voxel->Velocity *= .98;
			}

			if (depthTilesViolated == 4) {
				//The tile is fully underground, force it to the surface
				voxel->Position.z = lowestHeight+.5f; 
				voxel->Velocity.z = 2;
			}


			//Now apply velocity/acceleration
			//Always decrease the total energy in the system
			voxel->Velocity *= .99;
			//Apply forces!
			voxel->Velocity += voxel->Acceleration*(float)delta;
			voxel->Position += voxel->Velocity*(float)delta;
		}
	}

	//A tie to the voxel system used to lookup the terrain height at various points
	VoxelSystem * voxelSystem;

	//C style function for performance reasons
	friend Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition);
public:
	PhysicsSystem(VoxelSystem * system);
	~PhysicsSystem();

	//Traces a line to the first intersecting terrain or physics voxel
	bool Raytrace(vec3 from, vec3 direction, vec3 & rayCollision, vec3 & surfaceNormal);

	//Constructs a voxel at the given coordinate
	//returns the voxel
	PhysicsVoxel * BuildVoxel(vec3 voxelCoordinate, double lifeTime=-1.0);

	//Used by PhysicsActor to register/unregister itself. Not needed elsewhere
	void RegisterPhysicsActor(PhysicsActor * toRegister);
	void UnregisterPhysicsActor(PhysicsActor * toUnregister);

	//Update the physics voxels, called by base frame
	void Update(double delta, double now);

	//Draw all the actors
	void Draw(ShaderGroup * shaders);

	//Save system information
	CLASS_DECLARATION(PhysicsSystem)
		CLASS_CONTAINER_MEMBER(allVoxels,ReflectionData::SAVE_CONTIGOUSLIST,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};