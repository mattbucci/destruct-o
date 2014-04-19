
#pragma once

#include "stdafx.h"

typedef int FactionId;

class GameFactions : public Savable{
	//Map all allies to all others
	set<pair<FactionId,FactionId>> allies;
protected:
	//To save allies use manual save/load embedded in these functions
	virtual void Save(Json::Value & parentValue) override;

	virtual void Load(Json::Value & parentValue, LoadData & loadData) override;
public:
	//Default factions
	//Only the player and direct extensions of the player is on this team
	static const FactionId FACTION_PLAYER;
	//AI allied to the player is on this team
	static const FactionId FACTION_PLAYERALLY;
	//Generic AI enemies are on this team
	static const FactionId FACTION_ENEMY;
	//Enemies hostile to all are on this team, or any other
	static const FactionId FACTION_HOSTILE;
	//The terrain (such is if you fall from a great height or a physics voxel hits you)
	static const FactionId FACTION_EARTH;
	//To make an actor hostile to all other actors
	//just randomly assign a faction id

	GameFactions();

	//Check if two factions are allies
	bool IsAlly(FactionId a, FactionId b);
	//Check if two factions are enemies
	bool IsEnemy(FactionId a, FactionId b);

	//Make two factions allies
	void SetAllies(FactionId a, FactionId b);
	//Make two factions enemies
	void SetEnemies(FactionId a, FactionId b);

	//GameFactions list must be saved manually since its a special data type (set of pairs)
	CLASS_DECLARATION(ActorSystem)
	END_DECLARATION
};