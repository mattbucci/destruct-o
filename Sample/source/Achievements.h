/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
