#include "stdafx.h"
#include "PhysicsSystem.h"
#include "Actor.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "PhysicsVoxel.h"
#include "VoxelSystem.h"

PhysicsSystem::PhysicsSystem(VoxelSystem * system) {
	lastEmpty = 0;
	highestFull = 0;
	//Build a voxel renderer
	renderer = VoxelDrawSystem::BuildAppropriateSystem();
	voxelSystem = system;

}
PhysicsSystem::~PhysicsSystem() {
	//cleanup all the actors
	//they auto-deregister, so we have to iterate over a separate list
	vector<PhysicsVoxel*> voxels = allVoxels;
	for (auto voxel : voxels)
		delete voxel;
}


PhysicsSystem::Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition) {
	PhysicsSystem::Intersection data;

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

//Should be called by Actor.cpp only
//no one else call these
void PhysicsSystem::Register(PhysicsVoxel * toRegister) {
	//Search for an empty space in allVoxels
	//accelerated by the lastEmpty property
	for (unsigned int i = lastEmpty; i < allVoxels.size(); i++) {
		if (allVoxels[i] == NULL) {
			//found an empty space
			allVoxels[i] = toRegister;
			//Mark the space as full
			if (i > highestFull)
				highestFull = i;
			//you know there's nothing empty below this
			lastEmpty = i+1;
			return;
		}
	}
	//Expand the actor array
	//note there is currently no way to shrink the actor away
	//so small bursts of large numbers of actors will cause array expansion
	allVoxels.push_back(toRegister);
	highestFull = allVoxels.size()-1;
	lastEmpty = allVoxels.size();
}
void PhysicsSystem::Unregister(PhysicsVoxel * toUnregister) {
	//Do a linear search for the actor to unregister
	//This could easily be optimized to be O(1) if actors
	//remembered their position in the actor array
	for (unsigned int i = 0; i < allVoxels.size(); i++) {
		if (allVoxels[i] == toUnregister) {
			allVoxels[i] = NULL;
			//remember this position as empty
			if (i < lastEmpty)
				lastEmpty = i;

			return;
		}
	}
	//Should never get here
	_ASSERTE(false);
}

PhysicsVoxel * PhysicsSystem::BuildVoxel(vec3 voxelCoordinate) {
	PhysicsVoxel * voxel = new PhysicsVoxel();
	voxel->Position = voxelCoordinate + vec3(.5,.5,.5);
	Register(voxel);
	cout << "Physics Voxel Count: " << allVoxels.size() << "\n";
	return voxel;
}
float removeInDirection(vec3 & force, vec3 direction) {
	float d = glm::dot(force,direction);
	if (d > 0) {
		force -= direction*d;
		return d;
	}
	return 0.0f;
}

//Update the actors, called by base frame
void PhysicsSystem::Update(double delta, double now) {
	unsigned int newFullSize = 0;

	//No actors mean nothing to do
	if (allVoxels.size() <= 0)
		return;

	//Set all voxels to not colliding
	for (unsigned int i = 0; i <= highestFull; i++) {
		if (allVoxels[i] != NULL) {
			newFullSize = i;
			allVoxels[i]->colliding = false;
			allVoxels[i]->Acceleration = vec3();
		}
	}


	//Now do the O(n^2) which checks for collisions
	for (unsigned int a = 0; a <= highestFull; a++) {
		for (unsigned int b = a+1; b <= highestFull; b++) {
			//Do AABB before you do full physics check... or things will be slow
			if (allVoxels[a]->AabColiding(allVoxels[b]->Position)) {
				Intersection intr = CalculateIntersection(allVoxels[a]->Position,allVoxels[b]->Position);


				allVoxels[a]->colliding = allVoxels[b]->colliding = true;
				float depth = intr.Depth;
				float force = 500*depth;

				//No two voxels can occupy the same position, push one voxel a little out of the other
				if (allVoxels[a]->Position == allVoxels[b]->Position)
					allVoxels[b]->Position += vec3(.01,.01,.01);

				vec3 forceDirection = intr.Normal;
				//one idea is to mix the ideal direction (face aligned) with 
				//the natural direction (collision aligned)
				//to add instabilities which cause blocks on the edge to fall over


				allVoxels[a]->Acceleration += forceDirection*force;
				allVoxels[b]->Acceleration += -forceDirection*force; 
				//Remove Velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(allVoxels[a]->Velocity,-forceDirection);
				vel += removeInDirection(allVoxels[b]->Velocity,forceDirection);
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 3.0f;
				allVoxels[a]->Velocity += vel*forceDirection;
				allVoxels[b]->Velocity += vel*-forceDirection;
				//Apply enhanced friction while they're touching
				allVoxels[a]->Velocity *= .99;
				allVoxels[b]->Velocity *= .99;
			}
		}
	}
	//set the current voxel to 0



	//Update all the voxels
	for (unsigned int i = 0; i <= highestFull; i++) {
		if (allVoxels[i] != NULL) {
			int a = i;

			allVoxels[a]->Acceleration += vec3(0,0,-10);
			
			if (allVoxels[a]->Position.z < 1.5) {
				//Current ground collision code
				//must be repaired to use height lookup in voxel location
				float depth = 1.5 - allVoxels[a]->Position.z;
				if (allVoxels[a]->Velocity.z < 0)
					allVoxels[a]->Velocity.z = 0;
				if (allVoxels[a]->Acceleration.z < 0)
					allVoxels[a]->Acceleration.z = 0;

				//Apply additional friction
				allVoxels[a]->Velocity *= .98;
			}


			//Do ground collisions now
			//There are (usually) four different tiles you're over
			vec2 floorTiles[4];
			floorTiles[0] = glm::floor(vec2(allVoxels[i]->Position));
			floorTiles[1] = vec2(floor(allVoxels[i]->Position.x),ceil(allVoxels[i]->Position.y));
			floorTiles[2] = vec2(ceil(allVoxels[i]->Position.x),floor(allVoxels[i]->Position.y));
			floorTiles[3] = vec2(ceil(allVoxels[i]->Position.x),ceil(allVoxels[i]->Position.y));
			//This is the direction which points the way the block's velocity/force will be reflected
			vec3 terrainRejectionDirection = vec3();
			//If the standard (axis aligned) directions cancel eachother out, use this one instead
			vec3 roughRejectionDirection = vec3();
			float penetrationDepth = 0;
			int penetrationCount = 0;
			for (int i = 0; i < 4; i++) {
				//check if the square is under the ground for this grund tile
				float height = voxelSystem->GetPositionHeight(floorTiles[i]);
				if ((height+.5) < allVoxels[a]->Position.z)
					continue;
				//So the block must be penetrating this block of terrain
				//time to reject it
				//simulate a block next to the penetrating voxel
				Intersection intersectionData = CalculateIntersection(allVoxels[a]->Position,vec3(floorTiles[i],min(height+.5f,allVoxels[a]->Position.z))+vec3(.5,.5,0));
				//If the depth is < 0 then you're not penetrating quite yet...
				if (intersectionData.Depth < 0)
					continue;
				terrainRejectionDirection += intersectionData.Normal;
				roughRejectionDirection += intersectionData.CollisionVector;
				penetrationDepth += intersectionData.Depth;
				penetrationCount++;
			}
			if (penetrationDepth > 0) {
				//A ground intersection occurred, reject the voxel
				float directionLength = glm::length(terrainRejectionDirection);
				if (directionLength < .01)
					terrainRejectionDirection = glm::normalize(roughRejectionDirection);
				else if (penetrationCount == 4)
					//If intersecting everything than the only way out is up
					terrainRejectionDirection = vec3(0,0,1);
				else
					//Otherwise normalize everything you're intersecting
					terrainRejectionDirection /= directionLength;
				//Apply force to reject the block
				allVoxels[a]->Acceleration += terrainRejectionDirection*penetrationDepth*100.0f;
				//Now reflect velocity
				float vel = 0.0f;
				vel += removeInDirection(allVoxels[a]->Velocity,-terrainRejectionDirection);
				//removeInDirection(allVoxels[a]->Acceleration
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 3.0f;
				allVoxels[a]->Velocity += vel*terrainRejectionDirection;
				//Apply enhanced friction while they're touching
				allVoxels[a]->Velocity *= .98;
			}

			//Now apply velocity/acceleration
			//Always decrease the total energy in the system
			allVoxels[a]->Velocity *= .99;
			//Apply forces!
			allVoxels[i]->Velocity += allVoxels[i]->Acceleration*(float)delta;
			allVoxels[i]->Position += allVoxels[i]->Velocity*(float)delta;
			//Track the highest valid physics voxel
			newFullSize = i;
		}
	}

	//Update the highest full size
	highestFull = newFullSize;

	//A slow (sort of) bubble sort could be used
	//to slowly compact the physics array
	//but I don't think that's necessary right now
}


//Draw all the actors
void PhysicsSystem::Draw(ShaderGroup * shaders) {
	GL3DProgram * shader = (GL3DProgram *)shaders->GetShader("3d");
	unsigned int newFullSize = 0;

	//No voxels mean nothing to do
	if (allVoxels.size() <= 0)
		return;

	//Translate to the voxel position
	shader->Model.PushMatrix();
	//Voxels store their position around the center of mass instead of their edge
	//add a correction to the model matrix
	shader->Model.Translate(vec3(-.5,-.5,-.5));
	shader->Model.Apply();

	renderer->startDraw(shader);

	//Draw all the voxels
	for (unsigned int i = 0; i <= highestFull; i++) {
		if (allVoxels[i] != NULL) {
			//Draw the voxel
			renderer->pushVoxel(shader,allVoxels[i]->Position,1);
		}
	}

	renderer->finishDraw(shader);

	shader->Model.PopMatrix();
}