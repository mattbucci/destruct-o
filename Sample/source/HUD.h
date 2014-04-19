
#pragma once

#include "stdafx.h"
#include "TexturedRect.h"

class ActorSystem;
class BaseFrame;
class ActorPlayer;
class GL2DProgram;
class FirstPersonMode;
class PhysicsActor;

class HUD {
	ContiguousList<pair<double, vec2>> damagePoints;

	ActorSystem* actorSystem;
	BaseFrame* baseFrame;
	ActorPlayer* player;
	FirstPersonMode* fps;
	ContiguousList<PhysicsActor*>* actors;

	float minimapScale;

	TexturedRect damageIndicator;
	TexturedRect minimapDot;
	TexturedRect minimapBackground;
	TexturedRect chargeBar;
	TexturedRect chargeBarBG;

	HUD();
public:
	HUD(BaseFrame* baseFrame);
	//Since the hud is purely visual
	//there is no point in updating it at 100hz
	//it might as well update as its drawn
	void DrawAndUpdate(GL2DProgram * shader, vec2 viewPortSize);

	void MarkDamage(vec2 source);
};