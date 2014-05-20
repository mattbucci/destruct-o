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
		INHERITS_FROM(ActorProjectile)
		CLASS_MEMBER(bomb,ReflectionData::SAVE_OWNEDHANDLE);
	END_DECLARATION
};