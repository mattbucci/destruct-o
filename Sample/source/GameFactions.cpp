#include "stdafx.h"
#include "GameFactions.h"

//Default factions
//Only the player and direct extensions of the player is on this team
const FactionId GameFactions::FACTION_PLAYER = 0;
//AI allied to the player is on this team
const FactionId GameFactions::FACTION_PLAYERALLY = 1;
//Enemies hostile to all are on this team, or any other
const FactionId GameFactions::FACTION_HOSTILE = 3;
//The terrain (such is if you fall from a great height or a physics voxel hits you)
const FactionId GameFactions::FACTION_EARTH = 4;
//AI factions start at this and go up
const FactionId GameFactions::FACTION_AIFACTION = 100;
//The number of AI factions
const int GameFactions::AI_FACTION_COUNT = 5;
//To make an actor hostile to all other actors
//just randomly assign a faction id

GameFactions::GameFactions() {
	//Create normal alliances
	SetAllies(FACTION_PLAYER,FACTION_PLAYERALLY);
}


//To save allies use manual save/load embedded in these functions
void GameFactions::Save(Json::Value & parentValue) {
	Json::Value & allySets = parentValue["allies"];
	int allyNumber = 0;
	//save each ally
	for (auto allyPair : allies) {
		Json::Value & pair = allySets[allyNumber];
		pair["first"] = allyPair.first;
		pair["second"] = allyPair.second;
		allyNumber++;
	}
}

void GameFactions::Load(Json::Value & parentValue, LoadData & loadData) {
	//Clear existing allies
	allies.clear();

	//Load new ones
	Json::Value & allySets = parentValue["allies"];
	for (auto ally : allySets) {
		allies.insert(pair<FactionId,FactionId>(ally["first"].asInt(),ally["second"].asInt()));
	}
}

//Retrieve the color of the given faction
vec4 GameFactions::FactionColor(FactionId faction) {
	static const vec4 factionColorLookup[] = {
		vec4(0,0,1,1),
		vec4(1,1,0,1),
		vec4(1,0,1,1),
		vec4(.5,0,.75,1),
		vec4(0,1,1,1),
	};
	//The player's faction is green
	if (faction == FACTION_PLAYERALLY)
		return vec4(0,1,0,1);
	//Check if the faction is an AI faction
	if ((faction < FACTION_AIFACTION) || (faction >= (FACTION_AIFACTION+AI_FACTION_COUNT)))
		return vec4();
	return factionColorLookup[faction-FACTION_AIFACTION];
}

//Check if two factions are allies
bool GameFactions::IsAlly(FactionId a, FactionId b) {
	return (a == b) || allies.find(pair<FactionId,FactionId>(a,b)) != allies.end();
}
//Check if two factions are enemies
bool GameFactions::IsEnemy(FactionId a, FactionId b) {
	return (a != b) && allies.find(pair<FactionId,FactionId>(a,b)) == allies.end();
}

//Make two factions allies
void GameFactions::SetAllies(FactionId a, FactionId b) {
	if (!IsAlly(a,b)) {
		allies.insert(pair<FactionId,FactionId>(a,b));
		allies.insert(pair<FactionId,FactionId>(b,a));
	}
		
}
//Make two factions enemies
void GameFactions::SetEnemies(FactionId a, FactionId b) {
	if (IsAlly(a,b)) {
		allies.erase(allies.find(pair<FactionId,FactionId>(a,b)));
		allies.erase(allies.find(pair<FactionId,FactionId>(b,a)));
	}
		
}