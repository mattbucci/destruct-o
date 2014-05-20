#include "stdafx.h"
#include "Demo.h"
#include "Frames.h"
#include "BaseFrame.h"
#include "PhysicsVoxel.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "ParticleData.h"
#include "Utilities.h"
#include "Universal.h"

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
				cubeParts[x+y*2+z*4] = game->Physics.BuildVoxel(cubePos + vec3(x*sepDist,y*sepDist,z*sepDist),Utilities::random(8.0,10.0));
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

void Demo::DoInitialSave() {
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	game->Save("reset_save.json.compressed");
}

void Demo::OnInput(vector<InputEvent> events, vec3 playerPos, vec3 playerFacing) {
	playerPos += vec3(0,0,2);
	BaseFrame * game = (BaseFrame*)CurrentSystem;
	for (auto eve : events) {
		if (eve.Event == InputEvent::KeyboardDown) {
			if (eve.Key == SDLK_ESCAPE) {
				game->ToggleMenu();
			}
			//Fire blocks
			if (eve.Key == 'f') {
				PhysicsVoxel * vox = game->Physics.BuildVoxel(playerPos+playerFacing*2.0f,Utilities::random(8.0,10.0));
				vox->Velocity = game->FirstPerson->GetLookVector()*50.0f;
			}
			else if (eve.Key == 'e') {
				PhysicsVoxel * vox = game->Physics.BuildVoxel(playerPos+vec3(0,0,2),Utilities::random(8.0,10.0));
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
							game->Physics.BuildVoxel(cubePos+vec3(x*.5,y*.5,rand() % 5+1) + randomness,Utilities::random(8.0,10.0));
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
							game->Physics.BuildVoxel(cubePos+vec3(a,b,2+3*p),Utilities::random(8.0,10.0));
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
							game->Physics.BuildVoxel(cubePos+vec3(a,b,2+3*p)+randomness,Utilities::random(8.0,10.0) );
						}
					}
				}
			}
			else if (eve.Key == 'z') {
				const float initialEnergy = 20.0f;
				const float initialDisplacement = 0.5f;
				vec3 hit;
				
				if (Universal::Trace(playerPos+vec3(0,0,2.5),playerFacing,&hit)) {

					vector<vec4> displacedVoxels = game->Voxels.Crater(hit,5,20);
					for (vec4 & voxRemoved : displacedVoxels) {
						vec3 vox = vec3(voxRemoved);
						//add a bit of random noise
						vox += vec3(Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement),Utilities::random(-initialDisplacement,initialDisplacement));
						//Build the voxel
						PhysicsVoxel * ph = game->Physics.BuildVoxel(vox,Utilities::random(4.0,6.0));
						//Set the material
						ph->MaterialId = (int)voxRemoved.w;
						//and some energy (velocity)
						ph->Velocity = vec3(Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy),Utilities::random(-initialEnergy,initialEnergy));
                        
					}
                    vec3 position = hit - vec3(0, 0, 1);
                    CraterCalled.Fire([position](function<void(vec3)> subscriber) {
                        subscriber(position);
                    });

				}
					
			}
			else if (eve.Key == 'x') {
				const float initialEnergy = 50.0f;
				const float initialDisplacement = 2.0f;
				vec3 hit;
				if (Universal::Trace(playerPos + vec3(0, 0, 2.5), playerFacing, &hit)) {
					/*
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
                    
                    CraterCalled.Fire([testSystem](function<void(vec3)> subscriber) {
                        subscriber(testSystem->Position);
                    });


					vector<vec4> displacedVoxels = game->Voxels.Crater(hit, 5,20);
					for (vec4 & voxRemoved : displacedVoxels) {
						vec3 vox = vec3(voxRemoved);
						//add a bit of random noise
						vox += vec3(Utilities::random(-initialDisplacement, initialDisplacement), Utilities::random(-initialDisplacement, initialDisplacement), Utilities::random(-initialDisplacement, initialDisplacement));
						//Build the voxel
						PhysicsVoxel * ph = game->Physics.BuildVoxel(vox,Utilities::random(4.0,6.0));
						//Set the material
						ph->MaterialId = (int)voxRemoved.w;
						//and some energy (velocity)
						ph->Velocity = vec3(Utilities::random(-initialEnergy, initialEnergy), Utilities::random(-initialEnergy, initialEnergy), Utilities::random(-initialEnergy, initialEnergy));
					}
                    */
				}
			}
			else if (eve.Key == 't') {
				//takeover an area
				vec3 hit;
				int size = 20;
				if(Universal::Trace(playerPos + vec3(0, 0, 2.5), playerFacing, &hit)) {
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
                vec3 hit;
                if (Universal::Trace(playerPos + vec3(0, 0, 2.5), playerFacing, &hit)) {
                    /*rules = ParticleData::LoadParticleData("particles/fire.vpart");
                    if (rules == NULL)
                        return;

                    ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules, 5);
                    testSystem->Position = hit;*/
                }

			}
            else if (eve.Key == '`') {
                vec3 cubePos = playerPos + playerFacing*5.0f;

                //Yes you leak a particle data every time you do this
                //should be fixed
                ParticleData * rules;
                vec3 hit;
                if (Universal::Trace(playerPos + vec3(0, 0, 2.5), playerFacing, &hit)) {
                   /* rules = ParticleData::LoadParticleData("particles/exampleAnimated.vpart");
                    if (rules == NULL)
                        return;

                    ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules, 10);
                    testSystem->Position = hit;*/
                }

            }
			else if (eve.Key == '6') {
				vec3 cubePos = playerPos+playerFacing*20.0f;

				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules;
				/*
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
                CraterCalled.Fire([testSystem](function<void(vec3)> subscriber) {
                    subscriber(testSystem->Position);
                });
				*/
			}
			else if (eve.Key == '7') {
				/*
				vec3 cubePos = playerPos+playerFacing*5.0f;

				//Yes you leak a particle data every time you do this
				//should be fixed
				ParticleData * rules;

				rules = ParticleData::LoadParticleData("particles/blood.vpart");
				if (rules == NULL)
					return;

				ParticleSystem * testSystem = game->Particles.BuildParticleSystem(*rules,.1);
				testSystem->Position = cubePos;
				*/
			}
			else if (eve.Key == 'r') {
				game->Load("reset_save.json.compressed");
			}
            else if (eve.Key == '0')
            {
                // Create a model instance from this model
                ModelInstance *instance = game->Models()->NewInstance("soldier01");
                
                // Set the location of this instance
                instance->GetTransform().Translation() = playerPos + playerFacing * 5.0f;
                
                // Set the rotation of this isntance
                instance->GetTransform().Rotation() = glm::quat(vec3(0.5 * M_PI, 0.0, 0.0));
                
                // Store this instance for rendering
                //game->modelInstances.push_back(instance);
                
            } else if (eve.Key == '\\') {
				vec3 hit, norm;
				if(Universal::Trace(playerPos + vec3(0, 0, 2.5), playerFacing, &hit)) {
					((BaseFrame*)CurrentSystem)->GetHUD()->MarkDamage(vec2(hit.x, hit.y));
				}
			} else if (eve.Key == '/') {
				VoxEngine::AccountOptions.Print();
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

void Demo::Update()
{

}

void Demo::Draw(GL3DProgram * shader)
{

} 