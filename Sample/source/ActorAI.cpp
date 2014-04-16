#include "stdafx.h"
#include "ActorAI.h"

static float actorAIX = 68.5;

ActorAI::ActorAI() : PhysicsActor(vec3(2,2,5)) {
	Position = (vec3(40,42,actorAIX));
	actorAIX += 10;
}
ActorAI::~ActorAI() {

}