//
//  Achievements.h
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//

#ifndef __VoxEngine__Achievements__
#define __VoxEngine__Achievements__

#include "stdafx.h"

#include "GameEvent.h"

class Achievement;
class AchievementProgress;
class Notification;
class BaseFrame;


class Achievements : protected GameEventSubscriber, public Savable{
public:
	//Never ever change the order of this enum
	//add new achievements at the end
	//this enum links the list of possible achievements (which is not saved)
	//to the list of achievement progress (which is always saved)
	typedef enum : int {
		ACH_BABYSTEPS,
		ACH_SMALLSTEPS,
		ACH_WHERESFIRE,
		ACH_LEARNINGTOFLY,
		ACH_DESTROYEROFWORLDS,
		//add more here
	} AchievementType;
private:
	//Add a possible achievement to the achievements list
	void AddPossibleAchievement(AchievementType uniqueType, float goalValue, int rewardPoints, string achievementType, string achievementText);

	//Add an achievement notifying the user if they completed it
	void incrementAchievement(const string & name, float incrementQuantity);

	//The list of possible achievements (not saved)
    map<AchievementType,Achievement*> achievementlist;
	//The list of achievement progress (saved)
	vector<AchievementProgress*> achievementProgress;
	//A map<> for faster lookup of achievement progress
	//generated from the vector above on load
	//why not just save this map? Saving maps not supported
	map<AchievementType,AchievementProgress*> progress;

	//notification system to notify things
    Notification * notify;
	//Total points earned through achievements
	int pointsEarned;
	//Last time the user was notified of the upgrade window
	double lastUpgradeNotification;
protected:
	//Overload to rebuild the progress map after this class is loaded
	void Load(Json::Value & parentValue, LoadData & loadData) override;

	//Build the progress map from the progress vector
	void buildProgressMap();
public:
    Achievements();
	~Achievements();

	//Unspent points earned so far
	//can be modified because the player can spend them
	int PointsUnspent;

	//Connect the achievements object to baseframe
	//should only be called once per program execution
	void ConnectToGame(Notification* notification, BaseFrame * game);
    
    GameEvent<void(Achievement*)> AchivementEarned;

	CLASS_DECLARATION(Achievements)
		CLASS_CONTAINER_MEMBER(achievementProgress,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_OWNEDHANDLE)
		CLASS_MEMBER(pointsEarned,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(PointsUnspent,ReflectionData::SAVE_INT32)
	END_DECLARATION
};

#endif /* defined(__VoxEngine__Achievements__) */
