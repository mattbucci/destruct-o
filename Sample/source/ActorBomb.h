#pragma once

#include "ActorProjectile.h"

class WeaponAI;

class ActorBomb : public ActorProjectile {
	WeaponAI * bomb;

public:
	ActorBomb();

	virtual ~ActorBomb();
	

	virtual bool Update() override;


		//Draw this actor
	//And any associated model
	//override draw to fix rotation issue with this model
	virtual void Draw(MaterialProgram * materialShader);

	CLASS_DECLARATION(ActorBomb)
		//ActorProjectile has no save table, so just list the physics actor
		INHERITS_FROM(PhysicsActor)
		CLASS_MEMBER(bomb,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};