#include "stdafx.h"
#include "ActorAids.h"
#include "ActorSystem.h"
#include "BaseFrame.h"

//All AI actions
#include "AidsActionDeploySoldier.h"

//500 is 5 seconds
//this depends on SIMULATION_DELTA
static const int intensityAveragePeriod = 500;
//The highest target value for intensity
static const float MaxTargetIntensity = 200.0f;
//The fastest the intensity can grow per second
static const float MaxIntensityGrowRate = 20.0f;
//The amount of time in seconds the player gets a break for in the start of the game
//The intensity slowly ramps up during this time period
static const float StartGameBreakLength = 100.0f;


CLASS_SAVE_CONSTRUCTOR(ActorAids)

ActorAids::ActorAids() :
	intensityAdded(intensityAveragePeriod) {
	spawnedNasties = false;

	//Register possible actions
	actionList[AidsActionDeploySoldier::GetIntensityValue()] = [](vec3 pos) {return new AidsActionDeploySoldier(pos);};

	//Random offset for the intensity
	intensityCalculationOffset = Utilities::random(-10,10);

	currentIntensity = 0;
	targetIntensity = 0;
}

//All of the devious logic goes here
bool ActorAids::Update() {

	//Calculate the target intensity using magic
	//Use game time as a modifier
	float timeModifier = (float)(min(Game()->Now(),(double)StartGameBreakLength))/StartGameBreakLength;

	//For now, the magic is really stupid
	targetIntensity = (1.0f+sin(fmod(Game()->Now()/100 + intensityCalculationOffset,M_PI*2.0)))/2.0f*MaxTargetIntensity;
	targetIntensity *= timeModifier;

	//update actions and current intensity
	for (auto it = actions.begin(); it != actions.end(); ) {
		if ((*it)->Update()) {
			//cleanup time
			currentIntensity -= (*it)->GetIntensityCost();
			delete *it;
			//remove the action
			it = actions.erase(it);
		}
		else
			it++;
	}

	//If the current intensity is higher than the target
	//nothing to do
	if (currentIntensity > targetIntensity) {
		intensityAdded.AddSample(0);
		return Actor::Update();
	}

	//Otherwise attempt to get to the target intensity
	//right now this done stupidly
	//by randomly selecting an action
	auto randomAction = actionList.begin();
    unsigned int randChoice = Utilities::random((unsigned int) 0, (unsigned int) actionList.size());
	if (randChoice >= actionList.size())
		randChoice = actionList.size()-1;
	advance(randomAction,randChoice);
	
	//Perform the selected action
	currentIntensity += randomAction->first;
	intensityAdded.AddSample(randomAction->first);
	cout << "AI Taking action[" << randChoice << "] with cost: " << randomAction->first << "\n";
	//Another dumb thing
	//always find the closest point to the player
	//should do something smarter than this
	actions.push_back(randomAction->second(Game()->Actors.Player()->GetPosition()));

	//Update the underlying actor
	return Actor::Update();
}



//An actor can request a pathing solution
void ActorAids::PathingSolutionRequest(PhysicsActor * requestingActor, vec2 toPoint) {
	//STUB
}

//An actor (probably one pending destruction) can cancel all requests
void ActorAids::CancelRequests(PhysicsActor * canclingActor) {
	//STUB
}