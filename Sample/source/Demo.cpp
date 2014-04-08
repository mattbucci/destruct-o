#include "stdafx.h"
#include "Demo.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "PhysicsVoxel.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "ParticleData.h"
#include "Utilities.h"

Demo::Demo() {
	CurrentAcidStrength = 0;
	queuedKeyPress = -1;
}

void Demo::CheckTouchInput(vec3 playerPos, vec3 playerFacing) {
	if (queuedKeyPress >= 0) {
		vector<InputEvent> fakeEvent;
		fakeEvent.push_back(InputEvent(InputEvent::KeyboardDown,0,queuedKeyPress));
		OnInput(fakeEvent,playerPos,playerFacing);
		queuedKeyPress =  -1;
	}
}

void Demo::QueueTouchEvent(int keyPress) {
	queuedKeyPress = keyPress;
}

void Demo::BuildCube(vec3 pos, float sepStrength) {
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	//Build a spring cube
	vec3 cubePos = pos;
	float dampCoeff = .5;
	float springCoeff = 20;
	float sepDist = 2;
	PhysicsVoxel * cubeParts[8];
	//Build the cubes
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				cubeParts[x+y*2+z*4] = game->Physics.BuildVoxel(cubePos + vec3(x*sepDist,y*sepDist,z*sepDist));
			}
		}
	}
	//Generate springs
	//cubeParts[(x % 2)+(y % 2)*2+(z % 2)*4]
	for (int x1 = 0; x1 < 2; x1++) {
		for (int y1 = 0; y1 < 2; y1++) {
			for (int z1 = 0; z1 < 2; z1++) {
				//Attach each cube to each other cube
				for (int x2 = 0; x2 < 2; x2++) {
					for (int y2 = 0; y2 < 2; y2++) {
						for (int z2 = 0; z2 < 2; z2++) {
							//Don't link cubes to themselves
							if ((x1!=x2)||(y1!=y2)||(z1!=z2)) {
								PhysicsVoxel * a = cubeParts[(x1 % 2)+(y1 % 2)*2+(z1 % 2)*4];
								PhysicsVoxel * b = cubeParts[(x2 % 2)+(y2 % 2)*2+(z2 % 2)*4];
								//game->Physics.AddSpringForce(a,b,glm::distance(a->Position,b->Position),dampCoeff,springCoeff);
							}
						}
					}
				}
			}
		}
	}
}

void Demo::OnInput(vector<InputEvent> events, vec3 playerPos, vec3 playerFacing) {
	playerPos += vec3(0,0,2);
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	for (auto eve : events) {
		if (eve.Event == InputEvent::KeyboardDown) {
			//Fire blocks
			if (eve.Key == 'f') {
				PhysicsVoxel * vox = game->Physics.BuildVoxel(playerPos+playerFacing*2.0f);
				vox->Velocity = game->FirstPerson->GetLookVector()*50.0f;
			}
			else if (eve.Key == 'e') {
				PhysicsVoxel * vox = game->Physics.BuildVoxel(playerPos+vec3(0,0,2));
				vox->Velocity = glm::normalize(playerFacing*2.0f+vec3(0,0,1))*20.0f;
			}
			else if (eve.Key == 'q') {
				//Spawn a cube
				vec3 cubeDir = playerFacing;
				cubeDir.z = 0;
				glm::normalize(cubeDir);
				BuildCube(playerPos+cubeDir*5.0f+vec3(0,0,5),10.0f);
			}
			else if (eve.Key == '1') {
				//Spawn a bunch of random cubes
				vec3 cubeDir = playerFacing;
				cubeDir.z = 0;
				glm::normalize(cubeDir);
				vec3 cubePos = playerPos+cubeDir*5.0f+vec3(0,0,5);
				for (int x = 0; x < 8; x+= (rand() % 4)) {
					for (int y = 0; y < 8; y+= (rand() % 4)) {
						vec3 randomness = vec3((rand() % 10)/100.0,(rand() % 10)/100.0,(rand() % 10)/100.0);
						if ((rand() % 2) == 1)
							game->Physics.BuildVoxel(cubePos+vec3(x*.5,y*.5,rand() % 5+1) + randomness);
					}
				}
			}
			else if (eve.Key == '2') {
				//Spawn a perfect pyramid
				vec3 cubeDir = playerFacing;
				cubeDir.z = 0;
				glm::normalize(cubeDir);
				vec3 cubePos = playerPos+cubeDir*5.0f+vec3(0,0,5);
				for (int p = 0; p < 3; p++) {
					for (int a =p; a < 6-p; a++) {
						for (int b = p; b < 6-p; b++) {
							//vec3 randomness = vec3((rand() % 10)/100.0,(rand() % 10)/100.0,(rand() % 10)/100.0);
							game->Physics.BuildVoxel(cubePos+vec3(a,b,2+3*p));
						}
					}
				}
			}
			else if (eve.Key == '3') {
				//Spawn a randomized pyramid
				vec3 cubeDir = playerFacing;
				cubeDir.z = 0;
				glm::normalize(cubeDir);
				vec3 cubePos = playerPos+cubeDir*5.0f+vec3(0,0,5);
				for (int p = 0; p < 3; p++) {
					for (int a =p; a < 6-p; a++) {
						for (int b = p; b < 6-p; b++) {
							vec3 randomness = vec3((rand() % 10)/100.0,(rand() % 10)/100.0,(rand() % 10)/100.0);
							game->Physics.BuildVoxel(cubePos+vec3(a,b,2+3*p)+randomness );
						}
					}
				}
			}
			else if (eve.Key == 'z') {
				const float initialEnergy = 20.0f;
				const float initialDisplacement = 0.5f;
				vec3 hit, norm;
				if (game->Physics.Raytrace(playerPos+vec3(0,0,2.5),playerFacing,hit,norm)) {

					vector<vec4> displacedVoxels = game->Voxels.Crater(hit,5);
					for (vec4 & voxRemoved : displacedVoxels) {
						vec3 vox = vec3(voxRemoved);
						//add a bit of random noise
						vox += vec3(Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement));
						//Build the voxel
						PhysicsVoxel * ph = game->Physics.BuildVoxel(vox);
						//Set the material
						ph->MaterialId = (int)voxRemoved.w;
						//and some energy (velocity)
						ph->Velocity = vec3(Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy));
					}

				}
					
			}
			else if (eve.Key == 'x') {
				const float initialEnergy = 50.0f;
				const float initialDisplacement = 2.0f;
				vec3 hit, norm;
				if (game->Physics.Raytrace(playerPos + vec3(0, 0, 2.5), playerFacing, hit, norm)) {

					ParticleData * rules;
					rules = ParticleData::LoadParticleData("particles/explosion.vpart");
					if (rules == NULL)
						return;
					ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules,.1);
					testSystem->Position = hit;
					rules = ParticleData::LoadParticleData("particles/debris.vpart");	
					if (rules == NULL)
						return;
					testSystem = game->Particles.BuildParticleSystem(*rules, .1);
					testSystem->Position = hit;
					rules = ParticleData::LoadParticleData("particles/smoke.vpart");	
					if (rules == NULL)
						return;
					testSystem = game->Particles.BuildParticleSystem(*rules, .1);
					testSystem->Position = hit - vec3(0, 0, 1);


					vector<vec4> displacedVoxels = game->Voxels.Crater(hit, 5);
					for (vec4 & voxRemoved : displacedVoxels) {
						vec3 vox = vec3(voxRemoved);
						//add a bit of random noise
						vox += vec3(Utilities::random(-initialDisplacement, initialDisplacement), Utilities::random(-initialDisplacement, initialDisplacement), Utilities::random(-initialDisplacement, initialDisplacement));
						//Build the voxel
						PhysicsVoxel * ph = game->Physics.BuildVoxel(vox);
						//Set the material
						ph->MaterialId = (int)voxRemoved.w;
						//and some energy (velocity)
						ph->Velocity = vec3(Utilities::random(-initialEnergy, initialEnergy), Utilities::random(-initialEnergy, initialEnergy), Utilities::random(-initialEnergy, initialEnergy));
					}
				}
			}
			else if (eve.Key == 't') {
				//takeover an area
				vec3 hit, norm;
				int size = 20;
				if(game->Physics.Raytrace(playerPos + vec3(0, 0, 2.5), playerFacing, hit, norm)) {
					for (int x = -size/2; x < size/2; x++) {
						for (int y = -size/2; y < size/2; y++) {
							game->Voxels.Paint(vec2(hit.x+x, hit.y+y), 3);
						}
					}
				}
			}
			else if (eve.Key == 'c') {
				SwitchDemo(0,playerPos,playerFacing);
			}
			else if (eve.Key == 'y') {
				CurrentAcidStrength += .2f;
				if (CurrentAcidStrength > 1.0f)
					CurrentAcidStrength = 1.0f;
			}
			else if (eve.Key == 'h') {
				CurrentAcidStrength -= .2f;
				if (CurrentAcidStrength < 0.0)
					CurrentAcidStrength = 0.0f;
			}
			else if (eve.Key == 'n') {
				((BaseFrame*)CurrentSystem)->Save("testsavefile.json.compressed");
			}
			else if (eve.Key == 'm') {
				((BaseFrame*)CurrentSystem)->Load("testsavefile.json.compressed");
			}
			else if (eve.Key == '5') {
				vec3 cubePos = playerPos+playerFacing*5.0f;

				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules;

				rules = ParticleData::LoadParticleData("particles/fire.vpart");
				if (rules == NULL)
					return;

				ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules,2);
				testSystem->Position = cubePos;
			}
			else if (eve.Key == '6') {
				vec3 cubePos = playerPos+playerFacing*20.0f;

				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules;

				rules = ParticleData::LoadParticleData("particles/explosion.vpart");
				if (rules == NULL)
					return;

				ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules,.1);
				testSystem->Position = cubePos;

				rules = ParticleData::LoadParticleData("particles/debris.vpart");
				if (rules == NULL)
					return;

				testSystem = game->Particles.BuildParticleSystem(*rules,.1);
				testSystem->Position = cubePos;

				rules = ParticleData::LoadParticleData("particles/smoke.vpart");	
				if (rules == NULL)
					return;

				testSystem = game->Particles.BuildParticleSystem(*rules, .1);
				testSystem->Position = cubePos - vec3(0, 0, 1);
			}
			else if (eve.Key == '7') {
				vec3 cubePos = playerPos+playerFacing*5.0f;

				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules;

				rules = ParticleData::LoadParticleData("particles/blood.vpart");
				if (rules == NULL)
					return;

				ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules,.1);
				testSystem->Position = cubePos;
			}
		}
	}
}

void Demo::SwitchDemo(int newDemo, vec3 playerPos, vec3 playerFacing) {
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	//First clear everything created in any previous demo
	//game->Physics.ClearVoxels();
	//game->Physics.ClearSprings();
	for (auto sys : systems) {
		delete sys;
	}
	systems.clear();
	//Next start the new demo
}

void Demo::Update(double now, double delta) {

}

void Demo::Draw(GL3DProgram * shader) {

} 