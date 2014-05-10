#pragma once

#include "ActorProjectile.h"

class WeaponAI;

class ActorRocket : public ActorProjectile {
	WeaponAI * bomb;

public:
	ActorRocket();

	virtual ~ActorRocket();
	

	virtual bool Update() override;


		//Draw this actor
	//And any associated model
	//override draw to fix rotation issue with this model
	//virtual void Draw(MaterialProgram * materialShader);

	CLASS_DECLARATION(ActorRocket)
		//ActorProjectile has no save table, so just list the physics actor
		INHERITS_FROM(ActorProjectile)
		CLASS_MEMBER(bomb,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};