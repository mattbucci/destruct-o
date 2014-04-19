#include "stdafx.h"
#include "ActorAI.h"

static float actorAIX = 68.5;

CLASS_SAVE_CONSTRUCTOR(ActorAI);

ActorAI::ActorAI() : PhysicsActor(vec3(2,2,5),100, GameFactions::FACTION_ENEMY) {
	Position = (vec3(40,42,actorAIX));
	actorAIX += 10;
}
ActorAI::~ActorAI() {

}