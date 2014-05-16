#pragma once

#include "PhysicsActor.h"
#include "GameEvent.h"
#include "GameCamera.h"
#include "WeaponAI.h"

class ActorPlayer : public PhysicsActor
{	
	// Change the current weapon to this weapon
	// Right now happens instantly
	void setWeapon(int weaponId);

	//delta since last walk fired
	double deltaPosition;
	
	// Weapon
	GameCamera       weaponCamera;
	Weapon          *currentWeapon;
	int currentWeaponId;
	
	// Standard player weaponry
	vector<WeaponAI*> weapons;
	
	//If the player fired the gun
	bool weaponFired;
	float maxEnergyPool;
	
	//Protect the player from actually dying
	virtual void onDeath() override;
	
	//When an actor is loaded
	//handles building weapons and model and such
	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;

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

	//Retrieve's the players charge pool
	float GetCharge();
	float GetMaxCharge();

	//The player is the only actor which consumes input events
	//void ReadInput(const vector<InputEvent> & events);
	bool debug;
	float debug_target_height;
	

	// Update the position based off the most recent movement and direction vectors
	bool Update() override;
	
	// Draw the weapon
	void DrawWeapon(MaterialProgram * materialShader);

	
	//Get modifiers for the selected weapon
	//A damage factor of 0 indicates the weapon is locked
	WeaponModifiers * GetModifiers(int weaponId);

	//Override the normal draw
	//and do nothing
	//this prevents the Actor from trying to draw the model
	//in a way that it shouldn't
	void Draw(MaterialProgram * materialShader) override;

	//Draw the effects of said weapon
	void Draw(GLEffectProgram * shader);
	
	// Get a reference to the weapon camera
	GameCamera& WeaponCamera();

	CLASS_DECLARATION(ActorPlayer)
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(debug,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(deltaPosition,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(debug_target_height,ReflectionData::SAVE_FLOAT);

		//Weapon data
		CLASS_CONTAINER_MEMBER(weapons,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(currentWeaponId,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(weaponFired,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(maxEnergyPool,ReflectionData::SAVE_FLOAT);
	END_DECLARATION
};