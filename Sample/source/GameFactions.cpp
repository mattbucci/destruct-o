#include "stdafx.h"
#include "GameFactions.h"

//Default factions
//Only the player and direct extensions of the player is on this team
const FactionId GameFactions::FACTION_PLAYER = 0;
//AI allied to the player is on this team
const FactionId GameFactions::FACTION_PLAYERALLY = 1;
//Generic AI enemies are on this team
const FactionId GameFactions::FACTION_ENEMY = 2;
//Enemies hostile to all are on this team, or any other
const FactionId GameFactions::FACTION_HOSTILE = 3;
//The terrain (such is if you fall from a great height or a physics voxel hits you)
const FactionId GameFactions::FACTION_EARTH = 4;
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

//Check if two factions are allies
bool GameFactions::IsAlly(FactionId a, FactionId b) {
	return allies.find(pair<FactionId,FactionId>(a,b)) != allies.end();
}
//Check if two factions are enemies
bool GameFactions::IsEnemy(FactionId a, FactionId b) {
	return allies.find(pair<FactionId,FactionId>(a,b)) == allies.end();
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