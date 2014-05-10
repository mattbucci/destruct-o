#include "stdafx.h"
#include "ActorAids.h"
#include "ActorSystem.h"
#include "BaseFrame.h"
#include "ActorPlayer.h"

//All AI actions
#include "AidsActionDeploySingle.h"

#include "ActorAI.h"

//500 is 5 seconds
//this depends on SIMULATION_DELTA
const int intensityAveragePeriod = 500;
//The highest target value for intensity
const float MaxTargetIntensity = 200.0f;
//The fastest the intensity can grow per second
const float MaxIntensityGrowRate = 5.0f;
//The amount of time in seconds the player gets a break for in the start of the game
//The intensity slowly ramps up during this time period
const float StartGameBreakLength = 20.0f;


//Cost of each single deployment
const float COST_DEPLOY_SOLDIER = 20.0f;
const float COST_DEPLOY_MECH = 30.0f;
const float COST_DEPLOY_HELI = 50.0f;
const float COST_DEPLOY_BOMBER = 60.0f;
const float COST_DEPLOY_ROCKETMECH = 45.0f;

CLASS_SAVE_CONSTRUCTOR(ActorAids)
	 
ActorAids::ActorAids() :
	intensityAdded(intensityAveragePeriod),
	intensity(100,20){
	spawnedNasties = false;

	//Register possible actions
	//actionList[COST_DEPLOY_SOLDIER] = [](vec3 pos) {return new AidsActionDeploySingle(pos,COST_DEPLOY_SOLDIER,"soldier.json");};
	//actionList[COST_DEPLOY_MECH] = [](vec3 pos) {return new AidsActionDeploySingle(pos,COST_DEPLOY_MECH,"mech.json");};
	//actionList[COST_DEPLOY_HELI] = [](vec3 pos) {return new AidsActionDeploySingle(pos,COST_DEPLOY_HELI,"heli.json");};
	//actionList[COST_DEPLOY_BOMBER] = [](vec3 pos) {return new AidsActionDeploySingle(pos,COST_DEPLOY_BOMBER,"bomber.json");};
	actionList[COST_DEPLOY_ROCKETMECH] = [](vec3 pos) {return new AidsActionDeploySingle(pos,COST_DEPLOY_ROCKETMECH,"rocketmech.json");};

	//Random offset for the intensity
	intensityCalculationOffset = Utilities::random(-10.0f,10.0f);

	baseIntensity = 0;
	targetIntensity = 0;
	cycleId = 0;
}

ActorAids::~ActorAids() {
	for (auto city : cities)
		delete city;
	cities.clear();
}

//Register a new city
//this is called during tile gen
//from the tile gen thread
//so city access is locked by the above mutex
void ActorAids::RegisterNewCity(SavableCityData * cityData) {
	lock_guard<mutex> guard(cityListMutex);
	cities.push_back(cityData);
}


//Populate nearby cities to the player with enemies/allies
void ActorAids::populateCities() {
	lock_guard<mutex> guard(cityListMutex);
	vec3 playerPosition = Game()->Actors.Player()->GetPosition();

	for (auto cityLP : cities) {
		SavableCityData & city = *cityLP;
		float cityDistance = glm::distance(playerPosition,city.cityPosition);
		//Check if you're too far away
		if (cityDistance > 300) {
			if (city.cityPopulation.size() > 0) {
				//Depopulate city
				for (auto actor : city.cityPopulation)
					actor->Damage(GameFactions::FACTION_EARTH,10000);
				city.cityPopulation.clear();
			}
		}
		else if (cityDistance < 225) {
			if ((city.cityPopulation.size() <= 0) && (city.gunPositions.size() > 0)) {
				//Populate city
				auto turretPosition = city.gunPositions.begin();
				auto turretAlive = city.gunAlive.begin();
				for (;turretPosition != city.gunPositions.end(); turretPosition++,turretAlive++) {
					if (*turretAlive) {
						//Create a turret
						ActorAI * turret = (ActorAI*)Game()->Actors.BuildAI(*turretPosition+vec3(0,0,20),"turret.json");
						//Align to the correct person
						turret->SetFaction(FindAIFactionOfPoint(vec2(*turretPosition)));
						city.cityPopulation.push_back(turret);
					}
				}
				//If the city is all dead don't try to populate it anymore
				if (city.cityPopulation.size() <= 0) {
					city.gunPositions.clear();
					city.gunAlive.clear();
				}
			}
		}

		//Check if the city has population
		if (city.cityPopulation.size() > 0) {
			//Check if anyone in the population is dead and if so remove them
			for (auto it = city.cityPopulation.begin(); it != city.cityPopulation.end(); ) {
				if ((*it)->Dead())
					it = city.cityPopulation.erase(it);
				else
					it++;
			}
			//Check if the city changed alliances
			if (!city.ownedByPlayer) {
				//If the tower is 15 voxels shorter than it used to be, the city is owned by the player
				if (Game()->Voxels.GetPositionHeight(city.cityCenterVoxel)+15 < city.originalCityCenterVoxel) {
					city.ownedByPlayer = true;
					//Change the alliances of anyone left in the city
					for (auto actor : city.cityPopulation)
						actor->SetFaction(GameFactions::FACTION_PLAYERALLY);
				}
			}
		}

	}
}

//All of the devious logic goes here
bool ActorAids::Update() {
	//Next cycle
	cycleId++;

	//Update cities
	populateCities();

	//Calculate the target intensity using magic
	//Use game time as a modifier
	float timeModifier = (float)(min(Game()->Now(),(double)StartGameBreakLength))/StartGameBreakLength;

	//For now, the magic is really stupid
	targetIntensity = (float)(1.0f+sin(fmod(Game()->Now()/100 + intensityCalculationOffset,M_PI*2.0)))/2.0f*MaxTargetIntensity;
	targetIntensity *= timeModifier;

	//update actions and current intensity
	for (auto it = actions.begin(); it != actions.end(); ) {
		if ((*it)->Update()) {
			//cleanup time
			baseIntensity -= (*it)->GetIntensityCost();
			delete *it;
			//remove the action
			it = actions.erase(it);
		}
		else
			it++;
	}

	//Calculate the current intensity right now
	float currentIntensity = baseIntensity;

	//Modify by the player's vital stats
	currentIntensity *= .5f+.5f*(1.0f - Game()->Actors.Player()->GetLifePercentage());

	//Add the instant intensity to the sample
	intensity.AddSample(currentIntensity);

	//If the current intensity is higher than the target
	//nothing to do
	if (currentIntensity > targetIntensity) {
		intensityAdded.AddSample(0);
		return Actor::Update();
	}

	//Enforce target intensity
	float targetIntensityRate = intensityAdded.GetAverage()/(SIMULATION_DELTA*intensityAveragePeriod);

	//If your intensity grew too fast, wait for it to slow down
	if (targetIntensityRate > MaxIntensityGrowRate) {
		intensityAdded.AddSample(0);
		return Actor::Update();
	}

	//Otherwise attempt to get to the target intensity
	//right now this done stupidly
	//by randomly selecting an action
	auto randomAction = actionList.begin();
	ActorAids::action_store::size_type randChoice = Utilities::random((unsigned int) 0, (unsigned int) actionList.size());
	if (randChoice >= actionList.size())
		randChoice = actionList.size()-1;
	advance(randomAction,randChoice);
	
	//Perform the selected action
	baseIntensity += randomAction->first;
	intensityAdded.AddSample(randomAction->first);
	cout << "AI Taking action[" << randChoice << "] with cost: " << randomAction->first << "\n";
	//Another dumb thing
	//always find the closest point to the player
	//should do something smarter than this
	actions.push_back(randomAction->second(Game()->Actors.Player()->GetPosition()));
	spawnedNasties = true;

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

//Given a position find the appropriate faction to assign to an actor at that position
//Basic rules:
//Inside a city, assign to that city
//Outside a city, assign to the nearest city in the direction of player->point
FactionId ActorAids::FindAIFactionOfPoint(vec2 point) {
	//If there are no cities, just make the enemy hostile to everything and everyone
	if (cities.size() <= 0)
		return GameFactions::FACTION_HOSTILE;

	float currentCityRating = -10000;
	FactionId currentCityFaction;
	
	vec2 player = vec2(Game()->Actors.Player()->GetPosition());

	//Check if inside a city
	for (auto city : cities) {
		float dist = glm::distance(vec2(city->cityPosition),point);
		//1.415 is ratio of radius to furthest edge of a square
		if (dist < 1.415f*citysize/2.0f)
			//Inside city
			return city->cityFaction;

		//Rate this city on how much sense it makes as a choice to use as the deciding city
		//for the faction of this
		float rating = 300.0f/dist;
		//Scale rating by the angle created by the following vectors:
		//vector 1: position of the player -> city
		//vector 2: position of the player -> point
		//the smaller the angle, the higher the scale
		vec2 vector1 = vec2(city->cityPosition) - player;
		vec2 vector2 = point - player;
		float angle = acos(glm::dot(vector1,vector2)/(glm::length(vector1)*glm::length(vector2)));
		//Nothing is > 180 degree difference
		angle = abs(angle);
		if (angle > M_PI)
			angle -= M_PI;
		//now produce a scaling value
		//this equation 1-((x-1)^3+1) where x is 0 to 1, produces a y 1 to 0, which quickly goes towards 0
		float angleFactor = 1-(pow((angle/M_PI)-1,3)+1);
		rating *= angleFactor;
		//Now check if this is the best rated or not
		if (currentCityRating < rating) {
			currentCityRating = rating;
			currentCityFaction = city->cityFaction;
		}
	}
	//Return the best rated city
	return currentCityFaction;
}

//Use the AIId
//to determine if that AI should do a heavy update, or a light update
bool ActorAids::DoHeavyCycle(unsigned int aiId) {
	return ((aiId + cycleId) % 10) == 0;
}