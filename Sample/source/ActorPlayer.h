#pragma once

#include "PhysicsActor.h"
#include "GameEvent.h"
#include "GameCamera.h"
#include "Weapon.h"
#include "WeaponStarter.h"

class ActorPlayer : public PhysicsActor
{	
	//delta since last walk fired
	double deltaPosition;
    
    // Weapon
    ModelInstance *weapon;
    GameCamera     weaponCamera;
	WeaponStarter testWeapon;

	//Energy pool
	float energyPool;

	//Weapon position
	//position of the weapon bone
	vec3 weaponBonePos;
	//Actual position of the weapon muzzle in world space
	vec3 weaponPos;
	//MVP from weapon space
	mat4 alteredMVP;
public:
	ActorPlayer();
	~ActorPlayer();

	//The player is the only actor which consumes input events
	//void ReadInput(const vector<InputEvent> & events);
	bool debug;
	float debug_target_height;
	
    // Build anything related to the actor (not GL safe, for things like models)
    void Build();
    
    // Update the position based off the most recent movement and direction vectors
	bool Update() override;
    
    // Draw the weapon
    void DrawWeapon(MaterialProgram * materialShader);

	//Draw the effects of said weapon
	void Draw(GLEffectProgram * shader);
    
    // Get a reference to the weapon camera
    GameCamera& WeaponCamera();

	CLASS_DECLARATION(ActorPlayer)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(deltaPosition,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);
	END_DECLARATION
};