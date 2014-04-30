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

//Include weapons for RTTI
#include "WeaponLaserCannon.h"
#include "WeaponPulseLaser.h"

//Register particle events
ParticleCloud::ParticleCloud(ActorSystem * actors, PhysicsSystem * physics) : Autocacher("particles/","particlemanifest.json") {

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
		ParticleSystem * testSystem = BuildParticleSystem(particlePuff, voxel->Position, Utilities::random(.3f,.5f));
	});
	//WEAPON EVENT HIT EVENTS
	Subscribe<void(Actor*,Weapon*,vec3)>(&actors->ActorWeaponImpact,[this](Actor * firingActor, Weapon * firedWeapon, vec3 hitPos) {
		//Mosquito weapon
		/*if (dynamic_cast<MosquitoAIWeapon*>(firedWeapon) != NULL) {
			ParticleData & particlePuff = Game()->Particles.GetCached("bulletLand.vpart");
			ParticleSystem * testSystem = BuildParticleSystem(particlePuff, .2f);
			testSystem->Position = hitPos;
		}
		//pulse laser
		else */if (dynamic_cast<WeaponPulseLaser*>(firedWeapon) != NULL) {
			ParticleData & particlePuff = Game()->Particles.GetCached("laserLand.vpart");
			particlePuff.Color.ClearValues();
			particlePuff.Color.AddValue(0,vec4(.1,.4,1,1));
			BuildParticleSystem(particlePuff,hitPos, .2f);
		}
		//laser cannon
		else if (dynamic_cast<WeaponLaserCannon*>(firedWeapon) != NULL) {
			ParticleData & particlePuff = Game()->Particles.GetCached("laserLand.vpart");
			particlePuff.Color.ClearValues();
			particlePuff.Color.AddValue(0,vec4(1,.5,.1,1));
			BuildParticleSystem(particlePuff,hitPos, .3f);
		}
	});
}
//Cleanup all systems
ParticleCloud::~ParticleCloud() {
	Clear();
}


//Cache particle data
ParticleData ParticleCloud::cacheItem(string path) {
	return  ParticleData::LoadParticleData(path);
}

//Updates the contents of the particle cloud automatically
void ParticleCloud::UpdateCloud() {

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

ParticleSystem * ParticleCloud::BuildParticleSystem(const ParticleData & particleType, vec3 pos, float lifeTime) {
	ParticleSystem * ps = new ParticleSystem(particleType,Game()->Now(),lifeTime);
	particles.push_back(ps);
	ps->Position = pos;
	return ps;
}

void ParticleCloud::Clear() {
	for (auto particleSystem : particles)
		delete particleSystem;
	particles.clear();
}


//Draw the particle cloud using a special shader in the future
void ParticleCloud::Draw(ShaderGroup * shaders) {
	GL3DProgram * shader3d = (GL3DProgram *)shaders->GetShader("3d");
	GLParticleProgram * shaderParticles = (GLParticleProgram*)shaders->GetShader("particles");
	//Switch to the particle shader
	shaderParticles->UseProgram();
	//Apply camera data from the 3d shader
	shaderParticles->ApplyCamera(shader3d->Camera);

    vec3 cameraZ = shader3d->Camera.GetZAxis();
    //size_t particleCount = 0;
    
	//Draw each particle system
	for (auto system : particles)
    {
        //particleCount += system->ParticleCount();
        system->Draw(&renderer, shaderParticles, cameraZ);
    }

    //cout << particleCount << " particles in " << particles.size() << " systems" << endl;
    
	//Some particle systems disable depth
	//re-enable it before continuing
	glDepthMask(GL_TRUE);
}
