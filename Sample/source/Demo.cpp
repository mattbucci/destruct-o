#include "stdafx.h"
#include "Demo.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "PhysicsVoxel.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "ParticleData.h"


Demo::Demo() {
	CurrentAcidStrength = 0;
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
			else if (eve.Key == 'r') {
				//Spawn a particle system
				vec3 cubePos = playerPos+playerFacing*5.0f;
				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules = new ParticleData();
				rules->GenerationRate.AddValue(0,40);
				rules->Velocity.AddValue(0,vec3(0,0,.5));
				rules->Variation.AddValue(0,.5);
				rules->Life.AddValue(0,10);
				rules->EmitterSize.AddValue(0,vec2(2,2));

				rules->Color.AddValue(0,vec4(1,0,0,1));
				rules->Color.AddValue(1,vec4(0,1,1,1));
				rules->Scale.AddValue(0,vec3(.2,.2,.2));
				rules->Scale.AddValue(1,vec3(.05,.05,.05));
				rules->Acceleration.AddValue(0,vec3());

				rules->ForceFunction = [](float timeSpan,vec3 position, vec3 velocity) {
					return 4.0f*vec3(sin(position.x),cos(position.y+position.z),sin(position.z*position.x));
				};
				//rules->Velocity
				ParticleSystem * testSystem = new ParticleSystem(rules,0.0);
				testSystem->Position = cubePos; 
				systems.push_back(testSystem);
			}
			else if (eve.Key == 'c') {
				SwitchDemo(0,playerPos,playerFacing);
			}
			else if (eve.Key == 'y') {
				CurrentAcidStrength += .2;
				if (CurrentAcidStrength > 1.0f)
					CurrentAcidStrength = 1.0f;
			}
			else if (eve.Key == 'h') {
				CurrentAcidStrength -= .2;
				if (CurrentAcidStrength < 0.0)
					CurrentAcidStrength = 0.0f;
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
	for (auto sys : systems) {
		sys->UpdateEmitter(now);
	}
}

void Demo::Draw(GL3DProgram * shader) {

}