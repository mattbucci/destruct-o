#include "stdafx.h"
#include "PhysicsSystem.h"
#include "Actor.h"
#include "GL3DProgram.h"
#include "ShaderGroup.h"
#include "PhysicsVoxel.h"
#include "VoxelSystem.h"
#include "PhysicsTriangle.h"
#include "VoxEngine.h"

PhysicsSystem::PhysicsSystem(VoxelSystem * system) {
	//Build a voxel renderer
	renderer = VoxelDrawSystem::BuildAppropriateSystem();
	voxelSystem = system;
	section.autoredeuce(false);
}
PhysicsSystem::~PhysicsSystem() {
	//cleanup all the voxels
	for (auto voxel : allVoxels)
		delete voxel;
}


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

PhysicsVoxel * PhysicsSystem::BuildVoxel(vec3 voxelCoordinate,double lifeTime) {
	PhysicsVoxel * voxel = new PhysicsVoxel();
	voxel->Position = voxelCoordinate + vec3(.5,.5,.5);
	voxel->MaterialId = 1;
	if (lifeTime <= 0)
		voxel->DeathAt = -1;
	else
		voxel->DeathAt = lifeTime + VoxEngine::GetGameSimTime();
	allVoxels.insert(voxel);
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


	//Set all voxels to have no forces
	//And check if any voxels should expire
	for (auto it = allVoxels.begin(); it != allVoxels.end();){
		PhysicsVoxel * voxel = *it;
		voxel->Acceleration = vec3();
		/*if ((voxel->DeathAt > 0) && (voxel->DeathAt < now)) {
			//disintegrate voxel
			//remove voxel
			it = allVoxels.erase(it);
		}
		else*/
			it++;
	}
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
		section.insert(allVoxels[s]);
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
	//set the current voxel to 0



	//Update all the voxels
	for (unsigned int a = 0; a < allVoxels.size(); a++) {
		if (allVoxels[a] != NULL) {

			allVoxels[a]->Acceleration += vec3(0,0,-10);
			
			if (allVoxels[a]->Position.z < 1.5) {
				//Current ground collision code
				//must be repaired to use height lookup in voxel location
				float depth = 1.5f - allVoxels[a]->Position.z;
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
			floorTiles[0] = glm::floor(vec2(allVoxels[a]->Position));
			floorTiles[1] = vec2(floor(allVoxels[a]->Position.x),ceil(allVoxels[a]->Position.y));
			floorTiles[2] = vec2(ceil(allVoxels[a]->Position.x),floor(allVoxels[a]->Position.y));
			floorTiles[3] = vec2(ceil(allVoxels[a]->Position.x),ceil(allVoxels[a]->Position.y));
			//Use these four blocks as a support for any blocks on top of the ground
			for (int i = 0; i < 4; i++) {
				//check if the square is under the ground for this ground tile
				float height = voxelSystem->GetPositionHeight(floorTiles[i]);
				if ((height+.5) < allVoxels[a]->Position.z)
					continue;


				//So the block must be penetrating this block of terrain
				//time to reject it
				//simulate a block next to the penetrating voxel
				Intersection intersectionData = CalculateIntersection(allVoxels[a]->Position,vec3(floorTiles[i],min(height-.5f,allVoxels[a]->Position.z))+vec3(.5,.5,0));
				//If the depth is < 0 then you're not penetrating quite yet...
				if (intersectionData.Depth < 0)
					continue;

				float force = 500*intersectionData.Depth;

				vec3 forceDirection = intersectionData.Normal;
				allVoxels[a]->Acceleration += forceDirection*force;
				//Remove Velocity in that direction
				float vel = 0.0f;
				vel += removeInDirection(allVoxels[a]->Velocity,-forceDirection);
				//The removed Velocity will now be thirded (instead of average since some is lost)
				//and added back in the opposite direction
				vel /= 3.0f;
				allVoxels[a]->Velocity += vel*forceDirection;
				//Apply extra enhanced friction while they're touching
				allVoxels[a]->Velocity *= .98;
			}


			//Now apply velocity/acceleration
			//Always decrease the total energy in the system
			allVoxels[a]->Velocity *= .99;
			//Apply forces!
			allVoxels[a]->Velocity += allVoxels[a]->Acceleration*(float)delta;
			allVoxels[a]->Position += allVoxels[a]->Velocity*(float)delta;
		}
	}
}

bool PhysicsSystem::checkForCollision(const vec3 & from, const vec3 & direction, vec3 at, vec3 & rayCollision, vec3 & surfaceNormal) {
	static PhysicsTriangle voxelTriangles[12] = {
		//Top
		PhysicsTriangle(vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3(1.0f,1.0f,1.0f),
		vec3(-1.0f,1.0f,1.0f),
		vec3(1.0f,0.0f,1.0f)),
		//Bottom
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f)),
		PhysicsTriangle(vec3(1.0f,1.0f,0.0f),
		vec3(-1.0f,1.0f,0.0f),
		vec3(1.0f,0.0f,0.0f)),
		//left
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 0.0f,1.0f,1.0f)),
		//right
		PhysicsTriangle(vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
		//front
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,0.0f),
		vec3( 1.0f,0.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,0.0f,0.0f),
		vec3( 0.0f,0.0f,1.0f),
		vec3( 1.0f,0.0f,1.0f)),
		//back
		PhysicsTriangle(vec3( 0.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,0.0f),
		vec3( 1.0f,1.0f,1.0f)),
		PhysicsTriangle(vec3( 0.0f,1.0f,0.0f),
		vec3( 0.0f,1.0f,1.0f),
		vec3( 1.0f,1.0f,1.0f)),
	};
	//Check every triangle in this voxel for a collision
	for (int i = 0 ; i < 12; i++) {
		double surf;
		vec3 norm;
		if (PhysicsTriangle::RayIntersects(voxelTriangles[i],at,from,direction,&surf,&norm)) {
			rayCollision = ((float)surf)*direction+from;
			surfaceNormal = norm;
			//Paint colliding voxels
			voxelSystem->Paint(vec2(at.x,at.y),1);
			//Stop checking voxels
			return true;
		}
	}
    return false;
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
		voxelSystem->Paint(vec2(x,y),5);
		//Check the voxel for a ray collision in 3d space
		//Check every voxel that's in this 2d region
		float height = voxelSystem->GetPositionHeight(vec2(x,y));
		int stackSize = voxelSystem->GetPositionStackSize(vec2(x,y));
		for (int stack = 0; stack <= stackSize; stack++) {
			if (checkForCollision(from,direction,vec3(x,y,height-(float)stack),rayCollision,surfaceNormal))
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
}