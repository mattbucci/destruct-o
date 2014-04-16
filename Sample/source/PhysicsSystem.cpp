#include "stdafx.h"
#include "PhysicsSystem.h"
#include "Actor.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "PhysicsVoxel.h"
#include "VoxelSystem.h"
#include "PhysicsTriangle.h"
#include "PhysicsActor.h"
#include "VoxEngine.h"
#include "Tracer.h"
#include "ActorPlayer.h"

template<class T>
T _trunc(T d){ return (d>0) ? floor(d) : ceil(d) ; }


//Remove the force in a given direction and return the quantity
static float removeInDirection(vec3 & force, vec3 direction) {
	float d = glm::dot(force,direction);
	if (d > 0) {
		force -= direction*d;
		return d;
	}
	return 0.0f;
}

#include "ParticleData.h"
#include "ParticleSystem.h"

#include "Frames.h"
#include "GameSystem.h"
#include "BaseFrame.h"

PhysicsSystem::PhysicsSystem(VoxelSystem * system) {
	//Build a voxel renderer
	renderer = VoxelDrawSystem::BuildAppropriateSystem();
	voxelSystem = system;
	section.autoredeuce(false);

	//For now stick this particle stuff here
    physicsVoxelErase = ParticleData::LoadParticleData("particles/physicsDisintegrate.vpart");
}
PhysicsSystem::~PhysicsSystem() {
	//cleanup all the voxels
	for (auto voxel : allVoxels)
		delete voxel;
}

//Given two voxel positions find intersection information
PhysicsSystem::Intersection CalculateIntersection(vec3 voxelAPosition, vec3 voxelBPosition) {
	PhysicsSystem::Intersection data;

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
PhysicsSystem::Intersection CalculateIntersection(vec3 voxelAPosition, vec3 cuboidPosition, vec3 halfCuboidSize) {
	PhysicsSystem::Intersection data;

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
PhysicsSystem::Intersection CalculateIntersection(vec3 cuboidPositionA, vec3 halfCuboidSizeA, vec3 cuboidPositionB, vec3 halfCuboidSizeB) {
	PhysicsSystem::Intersection data;

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


//Update and finalize physics actors
//also handles terrain interactions
void PhysicsSystem::updatePhysicsActors(float delta) {
	//Update all the physics voxels
	for (auto actor : actors) {
 		actor->acceleration += vec3(0,0,-10);
	
		vec3 actorPosition = actor->position;
		vec3 halfActorSize = actor->size*.5f;
		vec3 actorVolumeStart = actor->position - halfActorSize;
		vec3 actorVolumeEnd =	actor->position + halfActorSize;

		//Do ground collisions now
		//Iterate over every tile this actor is over
		//If the actor is fully below the surface of each floor tile checked, than it shall be teleported to the lowest surface
		int depthTilesViolated = 0;
		float lowestHeight = 300;

		int tilesChecked = 0;

		//Use these four blocks as a support for any blocks on top of the ground
		for (float atY = _trunc(actorVolumeStart.y);atY < actorVolumeEnd.y; atY++) {
			for (float atX = _trunc(actorVolumeStart.x);atX < actorVolumeEnd.x; atX++) {
				tilesChecked++;
				//check if the square is under the ground for this ground tile
				float height = voxelSystem->GetPositionHeight(vec2(atX,atY));
				if ((height+halfActorSize.z) < actor->position.z)
					continue;

				//Check if the depth constraint of this tile is being violated
				if ((height-2.5) > actor->position.z) {
					depthTilesViolated++;
					lowestHeight = min(lowestHeight,height);
				}
				
				//So the block must be penetrating this block of terrain
				//time to reject it
				//simulate a block next to the penetrating actor
				Intersection intersectionData = CalculateIntersection(vec3(vec2(atX,atY),min(height-.5f,actor->position.z))+vec3(.5,.5,0),actor->position,halfActorSize);
				//If the depth is < 0 then you're not penetrating quite yet...
				if (intersectionData.Depth < 0)
					continue;

				//Limit the force by the max number of voxels this object can encounter
				float forceLimiter = (actor->size.x*actor->size.y / 4.0f);
				float force = 500*intersectionData.Depth / forceLimiter;

				vec3 forceDirection = intersectionData.Normal;
				actor->acceleration += forceDirection*force;
				//Remove velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(actor->velocity,-forceDirection);
				//The removed velocity will now be forthed (instead of average since some is lost)
				//and added back in the opposite direction
				//actors bounce a lot less than blocks, so we /5 instead of /3
				vel /= 5.0f;
				vel /= forceLimiter;
				actor->velocity += vel*forceDirection;
				//Apply extra enhanced friction while they're touching
				actor->velocity *= .98;
				actor->onGround = true;
			}
		}
		
		if (depthTilesViolated == tilesChecked) {
			//The tile is fully underground, force it to the surface
			actor->position.z = lowestHeight+halfActorSize.z; 
			actor->velocity.z = 2;
		}
		

		//Now apply velocity/acceleration
		//Always decrease the total energy in the system
		actor->velocity *= .99;
		//Apply forces!
		actor->velocity += actor->acceleration*(float)delta;
		actor->position += actor->velocity*(float)delta;

		//Clear forces
		actor->acceleration = vec3();
	}
}

PhysicsVoxel * PhysicsSystem::BuildVoxel(vec3 voxelCoordinate,double lifeTime) {
	PhysicsVoxel * voxel = new PhysicsVoxel();
	voxel->Position = voxelCoordinate + vec3(.5,.5,.5);
	voxel->MaterialId = 1;
	if (lifeTime <= 0)
		voxel->DeathAt = -1;
	else
		voxel->DeathAt = lifeTime + VoxEngine::GetGameSimTime();
	allVoxels.push_back(voxel);
	return voxel;
}

//Update and finalize physics voxels
void PhysicsSystem::updatePhysicsVoxels(float delta) {
	//Update all the physics voxels
	for (auto voxel : allVoxels) {
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
void PhysicsSystem::collideVoxelsToVoxels(float delta) {
	allVoxels.sort([](PhysicsVoxel * a, PhysicsVoxel * b) -> bool {
		return a->Position.x < b->Position.x;
	});
	//Clear the valid section
	section.clear();
	//Use space partitioning to only check within sections
	for (unsigned int s = 0; s < allVoxels.size(); s++) {
		float limit = allVoxels[s]->Position.x;
		//Search for pieces that should no longer be in this section
		for (auto it = section.begin(); it != section.end();) {
			PhysicsVoxel * p = *it;
			if (abs(p->Position.x - limit) > 1.0)
				it = section.erase(it);
			else
				it++;
		}
		section.push_back(allVoxels[s]);
		if (section.size() > 50)
			cout << "";

		//Now do the O(n^2) which checks for collisions
		for (unsigned int a = 0; a < section.size(); a++) {
			for (unsigned int b = a+1; b < section.size(); b++) {
				//Do AABB before you do full physics check... or things will be slow
				if (section[a]->AabColiding(section[b]->Position)) {
					Intersection intr = CalculateIntersection(section[a]->Position,section[b]->Position);

					float depth = intr.Depth;
					float force = 100*depth;

					//No two voxels can occupy the same position, push one voxel a little out of the other
					if (section[a]->Position == section[b]->Position)
						section[b]->Position += vec3(.01,.01,.01);

					vec3 forceDirection = intr.Normal;
					//one idea is to mix the ideal direction (face aligned) with 
					//the natural direction (collision aligned)
					//to add instabilities which cause blocks on the edge to fall over


					section[a]->Acceleration += forceDirection*force;
					section[b]->Acceleration += -forceDirection*force; 
					//Remove Velocity in that direction
					float vel = 0.0f;
					vel += removeInDirection(section[a]->Velocity,-forceDirection);
					vel += removeInDirection(section[b]->Velocity,forceDirection);
					//The removed Velocity will now be thirded (instead of average since some is lost)
					//and added back in the opposite direction
					vel /= 3.0f;
					section[a]->Velocity += vel*forceDirection;
					section[b]->Velocity += vel*-forceDirection;
					//Apply enhanced friction while they're touching
					section[a]->Velocity *= .99;
					section[b]->Velocity *= .99;
				}
			}
		}
	}
}


void PhysicsSystem::collideVoxelsToActors(float delta) {
	//Now do the O(n^2) which checks for collisions
	for (auto actor : actors) {
		vec3 actorPosition = actor->position;
		vec3 halfActorSize = actor->size*.5f;
		vec3 actorVolumeStart = actor->position - halfActorSize;
		vec3 actorVolumeEnd =	actor->position + halfActorSize;

		for (auto voxel : allVoxels) {
			//Do AABB before you do full physics check... or things will be slow
			if (actor->aabbCollision(voxel->Position)) {
				Intersection intr = CalculateIntersection(voxel->Position,actorPosition,halfActorSize);

				float depth = intr.Depth;
				float force = 100*depth;

				//No two voxels can occupy the same position, push one voxel a little out of the other
				if (actor->Position == voxel->Position)
					voxel->Position += vec3(.01,.01,.01);

				vec3 forceDirection = -intr.Normal;
				//Check if the force will move the actor up
				if (intr.Normal.z > 0)
					actor->onGround = true;

				voxel->Acceleration += forceDirection*force;
				actor->Acceleration += -forceDirection*force; 
				//Remove Velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(voxel->Velocity,-forceDirection);
				vel += removeInDirection(actor->Velocity,forceDirection);
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 3.0f;
				voxel->Velocity += vel*forceDirection;
				actor->Velocity += vel*-forceDirection*.7f;
				//Apply enhanced friction while they're touching
				voxel->Velocity *= .99;
				actor->Velocity *= .99;
			}
		}
	}
}

void PhysicsSystem::collideActorsToActors(float delta) {
	//Now do the O(n^2) which checks for collisions
	for (int a = 0; a < actors.size(); a++) {
		PhysicsActor * actorA = actors[a];
		vec3 actorPositionA = actorA->position;
		vec3 halfActorSizeA = actorA->size*.5f;
		vec3 actorVolumeStartA = actorA->position - halfActorSizeA;
		vec3 actorVolumeEndA =	actorA->position + halfActorSizeA;

		for (int b = a+1; b < actors.size(); b++) {
			PhysicsActor * actorB = actors[b];

			vec3 actorPositionB = actorB->position;
			vec3 halfActorSizeB = actorB->size*.5f;
			vec3 actorVolumeStartB = actorB->position - halfActorSizeB;
			vec3 actorVolumeEndB =	actorB->position + halfActorSizeB;
			//Do AABB before you do full physics check... or things will be slow
			if (actorA->aabbCollision(actorB)) {
				Intersection intr = CalculateIntersection(actorPositionA,halfActorSizeA,actorPositionB,halfActorSizeB);

				float depth = intr.Depth;
				float force = 100*depth;

				//No two voxels can occupy the same position, push one voxel a little out of the other
				if (actorA->Position == actorB->Position)
					actorB->Position += vec3(.01,.01,.01);

				vec3 forceDirection = intr.Normal;
				//Check if the force will move the actor up
				if (intr.Normal.z > 0)
					actorB->onGround = true;
				if (intr.Normal.z < 0)
					actorA->onGround = true;

				actorB->Acceleration += forceDirection*force;
				actorA->Acceleration += -forceDirection*force; 
				//Remove Velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(actorB->Velocity,-forceDirection);
				vel += removeInDirection(actorA->Velocity,forceDirection);
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 4.0f;
				actorB->Velocity += vel*forceDirection;
				actorA->Velocity += vel*-forceDirection;
				//Apply enhanced friction while they're touching
				actorB->Velocity *= .99;
				actorA->Velocity *= .99;
			}
		}
	}
}

//Update the actors, called by base frame
void PhysicsSystem::Update(double delta, double now) {
	//Set all voxels to have no forces
	//And check if any voxels should expire
	for (auto it = allVoxels.begin(); it != allVoxels.end();){
		PhysicsVoxel * voxel = *it;
		voxel->Acceleration = vec3();
		if ((voxel->DeathAt > 0) && (voxel->DeathAt < now)) {
			//disintegrate voxel
			//this is temporary. will be replaced by an event soon
			if (voxel->MaterialId == 0) {
				//Do green color
				physicsVoxelErase->Color.ClearValues();
				physicsVoxelErase->Color.AddValue(0,vec4(.39,.761,.254,1));
				physicsVoxelErase->Color.AddValue(.5,vec4(.39,.761,.254,1));
				physicsVoxelErase->Color.AddValue(1,vec4(.39,.761,.254,1));
			}
			else {
				//Do brown color
				physicsVoxelErase->Color.ClearValues();
				physicsVoxelErase->Color.AddValue(0,vec4(.43,.304,.214,1));
				physicsVoxelErase->Color.AddValue(.5,vec4(.43,.304,.214,1));
				physicsVoxelErase->Color.AddValue(1,vec4(.43,.304,.214,1));
			}

			ParticleSystem * testSystem = ((BaseFrame*)CurrentSystem)->Particles.BuildParticleSystem(*physicsVoxelErase, .3);
			testSystem->Position = voxel->Position;
			//Notify any other systems that the voxel is being destroyed
			VoxelDisintegrating.Fire([voxel](function<void(PhysicsVoxel*)> subscriber) {
				subscriber(voxel);
			});
			delete voxel;
			//remove voxel
			it = allVoxels.erase(it);
		}
		else
			it++;
	}

	//Establish all actors as not on the ground
	//if they experience an upwards force as a result of a collision or ground, mark them as on the ground
	for (auto actor : actors)
		actor->onGround = false;
	
	//Now check for collisions between everything that can experience them
	collideVoxelsToVoxels((float)delta);
	collideVoxelsToActors((float)delta);
	collideActorsToActors((float)delta);
	//There are two missing
	//collideVoxelsToBuildings((float)delta);
	//collideActorsToBuildings((float)delta);

	//Now collide voxels and actors with the ground
	//then apply the accumulated acceleration on each
	updatePhysicsVoxels((float)delta);
	updatePhysicsActors((float)delta);
}


bool PhysicsSystem::Raytrace(vec3 from, vec3 direction, vec3 & rayCollision, vec3 & surfaceNormal) {
	//Ray trace in 2d to get a short list of possible colliding voxels from the terrain
	vec2 p0 = vec2(from);
	//Hits surfaces up to 200 away
	vec2 p1 = vec2(from+direction*200.0f);

	//2d ray tracing adapted from
	//http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
	vec2 d = glm::abs(p1-p0);

	int x = int(floor(p0.x));
	int y = int(floor(p0.y));

	int n = 1;
	int x_inc, y_inc;
	float error;

	if (d.x == 0) {
		x_inc = 0;
		error = std::numeric_limits<float>::infinity();
	}
	else if (p1.x > p0.x) {
		x_inc = 1;
		n += int(floor(p1.x)) - x;
		error = (floor(p0.x) + 1 - p0.x) * d.y;
	}
	else {
		x_inc = -1;
		n += x - int(floor(p1.x));
		error = (p0.x - floor(p0.x)) * d.y;
	}

	if (d.y == 0) {
		y_inc = 0;
		error -= std::numeric_limits<float>::infinity();
	}
	else if (p1.y > p0.y) {
		y_inc = 1;
		n += int(floor(p1.y)) - y;
		error -= (floor(p0.y) + 1 - p0.y) * d.x;
	}
	else {
		y_inc = -1;
		n += y - int(floor(p1.y));
		error -= (p0.y - floor(p0.y)) * d.x;
	}

	for (; n > 0; --n) {
		//Paint it to mark it as visited
		//voxelSystem->Paint(vec2(x,y),5);
		//Check the voxel for a ray collision in 3d space
		//Check every voxel that's in this 2d region
		float height = voxelSystem->GetPositionHeight(vec2(x,y));
		int stackSize = voxelSystem->GetPositionStackSize(vec2(x,y));
		for (int stack = 0; stack <= stackSize; stack++) {
			if (Tracer::TraceToVoxel(from,direction,vec3(x,y,height-(float)stack),rayCollision,surfaceNormal))
				//Collided with terrain
				return true;
		}


		//Move to the next voxel
		if (error > 0) {
			y += y_inc;
			error -= d.x;
		}
		else {
			x += x_inc;
			error += d.y;
		}
	}

	//No collision found
	return false;
}


//Draw all the actors
void PhysicsSystem::Draw(ShaderGroup * shaders) {
	GL3DProgram * shader = (GL3DProgram *)shaders->GetShader("3d");
	shader->UseProgram();

	//Translate to the voxel position
	shader->Model.PushMatrix();
	//Voxels store their position around the center of mass instead of their edge
	//add a correction to the model matrix
	shader->Model.Translate(vec3(-.5,-.5,-.5));
	shader->Model.Apply();

	renderer->startDraw(shader);

	//Draw all the voxels
	for (unsigned int i = 0; i < allVoxels.size(); i++) {
		//Draw the voxel
		renderer->pushVoxel(shader,allVoxels[i]->Position,allVoxels[i]->MaterialId);
	}

	renderer->finishDraw(shader);

	shader->Model.PopMatrix();


	//FOR DEBUG
	//DRAW ACTOR EXTENTS
	for (auto actor : actors) {
		//Skip the player
		if (typeid(*(Actor*)actor) == typeid(ActorPlayer))
			continue;


		shader->Model.PushMatrix();

		//Translate to the position you intend to draw at
		shader->Model.Translate(actor->position - actor->size*.5f);

		//Rescale so one voxel drawn at 0,0,0 fills the entire area
		shader->Model.Scale(actor->size);

		shader->Model.Apply();

		//draw one voxel
		renderer->startDraw(shader);
		renderer->pushVoxel(shader,vec3(),5);
		renderer->finishDraw(shader);

		shader->Model.PopMatrix();
	}
}

void PhysicsSystem::RegisterPhysicsActor(PhysicsActor * toRegister) {
	actors.push_back(toRegister);
}
void PhysicsSystem::UnregisterPhysicsActor(PhysicsActor * toUnregister) {
	for (auto it = actors.begin(); it != actors.end(); it++) {
		if (*it == toUnregister) {
			actors.erase(*it);
			return;
		}
	}
}