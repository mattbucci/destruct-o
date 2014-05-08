#include "stdafx.h"
#include "ParticleCloud.h"
#include "ParticleSystem.h"
#include "ShaderGroup.h"
#include "GLParticleProgram.h"
#include "GL3DProgram.h"
#include "BaseFrame.h"
#include "lodepng.h"
#include "ParticleData.h"
#include "ActorSystem.h"
#include "PhysicsSystem.h"
#include "PhysicsVoxel.h"

#include "TimeStepGuard.h"

//Register particle events
ParticleCloud::ParticleCloud(ActorSystem * actors, PhysicsSystem * physics)
	: Autocacher("particles/","particlemanifest.json"),
	//Trackers not used yet
	updateTracker(SIMULATION_DELTA*.1f,SIMULATION_DELTA*.25f),
	drawTracker(1.0f/60.0f*.15f,1.0f/60.0f*.65f){

	//VOXEL DISINTEGRATE EVENT
	static const vec4 materialColors[16] = {
		vec4(.39,.761,.254,1),
		vec4(.43,.304,.214,1),
		vec4(80.0f/255.0f,205.0f/255.0f,210.0f/255.0f,1),
		//No colors set for anything else, make them all brown
		vec4(.43,.304,.214,1),
		vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),
		vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),
		vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),vec4(.43,.304,.214,1),
	};
	Subscribe<void(PhysicsVoxel*)>(&physics->VoxelDisintegrating,[this](PhysicsVoxel * voxel) {
		ParticleData particlePuff = Game()->Particles.GetCached("physicsDisintegrate.vpart");
		//disintegrate voxel
		particlePuff.Color.ClearValues();
		particlePuff.Color.AddValue(0,materialColors[voxel->MaterialId]);
		BuildParticleSystem(particlePuff, voxel->Position, Utilities::random(.3f,.5f));
	});
}
//Cleanup all systems
ParticleCloud::~ParticleCloud() {
	Clear();
}

//Erase some particle systems randomly based off of the given recommendation
void ParticleCloud::enactRecommendation(float recommendation, bool updateRecommendation) {
	if (recommendation < 1.0f) {
		int originalSystems = particles.size();
		int eliminatedSystems = 0;

		//Delete some voxels randomly until the recommendation improves
		float chanceNotDeleted = .7f+.3f*(float)recommendation; 
		for (auto it = particles.begin(); it != particles.end();){
			//Voxels removed in this fashion do not produce particles
			if (Utilities::random(0.0f,1.0f) > chanceNotDeleted) {
				delete *it;
				it = particles.erase(it);
				eliminatedSystems++;
			}
			else
				it++;
		}
		if (eliminatedSystems > 0)
			cout << "Eliminated (" << eliminatedSystems << "/" << originalSystems << ") particle systems to improve performance" << (updateRecommendation ? "[U]" : "[D]") <<"\n";
	}
}

//Cache particle data
ParticleData ParticleCloud::cacheItem(string path) {
	return  ParticleData::LoadParticleData(path);
}

//Updates the contents of the particle cloud automatically
void ParticleCloud::UpdateCloud() {
	TimeStepGuard guard(&updateTracker);
	//carefully control performance
	enactRecommendation(updateTracker.GetRecommendation(),true);

	//First update each particle
	for (auto it = particles.begin(); it != particles.end(); ) {
		ParticleSystem * current = *it;
		
		if (current->UpdateEmitter()) {
			//Cleanup that particle system
			delete current;
			//Remove it from the list
			it = particles.erase(it);
		}
		else
			//Move to the next particle
			it++;
	}
}

void ParticleCloud::BuildParticleSystem(const ParticleData & particleType, vec3 pos, float lifeTime) {
	ParticleSystem * ps = new ParticleSystem(particleType,Game()->Now(),lifeTime);
	particles.push_back(ps);
	ps->Position = pos;
}

void ParticleCloud::Clear() {
	for (auto particleSystem : particles)
		delete particleSystem;
	particles.clear();
}


//Draw the particle cloud using a special shader in the future
void ParticleCloud::Draw(ShaderGroup * shaders) {
	TimeStepGuard guard(&drawTracker);
	//carefully control performance
	enactRecommendation(drawTracker.GetRecommendation(),false);

	GL3DProgram * shader3d = (GL3DProgram *)shaders->GetShader("3d");
	GLParticleProgram * shaderParticles = (GLParticleProgram*)shaders->GetShader("particles");
	//Switch to the particle shader
	shaderParticles->UseProgram();
	//Apply camera data from the 3d shader
	shaderParticles->ApplyCamera(shader3d->Camera);

    vec3 cameraZ = shader3d->Camera.GetZAxis();
    


	//Draw each particle system
	for (auto system : particles)
        system->Draw(&renderer, shaderParticles, cameraZ);
    
	//Some particle systems disable depth
	//re-enable it before continuing
	glDepthMask(GL_TRUE);
}
