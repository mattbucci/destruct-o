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

#include "stdafx.h"

#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "Achievements.h"
#include "Achievement.h"
#include "AchievementProgress.h"


Achievements::Achievements(){
	//Add all possible achievements
	AddPossibleAchievement(ACH_BABYSTEPS,5,50,"player-walked","Baby Steps, 5 steps taken");
	AddPossibleAchievement(ACH_SMALLSTEPS,25,100,"player-walked","One Small Step for man, 25 steps taken");
	AddPossibleAchievement(ACH_WHERESFIRE,100,200,"player-walked","Where's the Fire?, 100 steps taken");
	AddPossibleAchievement(ACH_LEARNINGTOFLY,1,50,"player-jumped","Learning to fly, Jumped for the first time");
	AddPossibleAchievement(ACH_DESTROYEROFWORLDS,100,50,"voxel-destroyed","Destroyer of worlds, 100 voxels destroyed");

	//No points earned or unspent so far
	pointsEarned = PointsUnspent = 0;
	lastUpgradeNotification = -10000;
};

Achievements::~Achievements() {
	//Cleanup everything
	for (auto achievement : achievementlist)
		delete achievement.second;
	for (auto progress : achievementProgress)
		delete progress;
}

//Add a possible achievement to the achievements list
void Achievements::AddPossibleAchievement(AchievementType uniqueType, float goalValue, int rewardPoints, string achievementType, string achievementText) {
	achievementlist[uniqueType] = new Achievement(uniqueType,rewardPoints,goalValue,achievementText,achievementType);
}

//Overload to rebuild the progress map after this class is loaded
void Achievements::Load(Json::Value & parentValue, LoadData & loadData ) {
	//load class data
	Savable::Load(parentValue,loadData);
	//rebuild progress map
	buildProgressMap();
}

//Build the progress map from the progress vector
void Achievements::buildProgressMap() {
	progress.clear();
	for (auto prog : achievementProgress)
		progress[prog->UniqueId] = prog;
}

//Connect the achievements object to baseframe
//should only be called once per program execution
void Achievements::ConnectToGame(Notification* notification, BaseFrame * game) {
	//Achievement subscription starts here
	GameEventSubscriber::Subscribe<void(Actor*)>(&game->Actors.ActorJumped,[this](Actor* Object) {
		if (Object == Game()->Actors.Player())
			incrementAchievement("player-jumped",1);
	});

	GameEventSubscriber::Subscribe<void(Actor*)>(&game->Actors.ActorLanded,[this](Actor* Object) {
		if (Object == Game()->Actors.Player())
			incrementAchievement("player-landed",1);
	});

	GameEventSubscriber::Subscribe<void(Actor*)>(&game->Actors.ActorWalked,[this](Actor* Object) {
		if (Object == Game()->Actors.Player())
			incrementAchievement("player-walked",1);
	});
	GameEventSubscriber::Subscribe<void(PhysicsVoxel*)>(&game->Physics.VoxelDisintegrating,[this](PhysicsVoxel* Object) {
		incrementAchievement("voxel-destroyed",1);
	});
	this->notify = notification;
}

void Achievements::incrementAchievement(const string & name, float incrementQuantity) {
	//Apply to all achievements which use this flag
    for(auto achievement : achievementlist) {
		if (achievement.second->type == name) {
			//Find the progress value to go with this achievement
			AchievementProgress * p;
			auto progressIterator = progress.find(achievement.second->uniqueAchievementId);
			if (progressIterator == progress.end()) {
				//No progress exists for this achievement, build a progress class
				p = new AchievementProgress(achievement.first);
				//add it to the progress map
				progress[achievement.first] = p;
				//add it to the progress vector
				achievementProgress.push_back(p);
			}
			else
				p = progressIterator->second;

			//If you've earned it you've got nothing else to do
			if (p->Earned)
				continue;

			p->Progress += incrementQuantity;
			//Check if they've earned it now
			//if they have mark it as earned and notify the user
			if (p->Progress >= achievement.second->targetvalue) {
				p->Earned = true;
				notify->Notify(string("Achievement Unlocked: ") + achievement.second->displaytext);
				//Add associated points
				PointsUnspent += achievement.second->rewardPoints;
				pointsEarned += achievement.second->rewardPoints;
				if ((Game()->Now() - lastUpgradeNotification) > 60) {
					//If it's been longer than 60 seconds since the last reminder
					//remind the player again
					lastUpgradeNotification = Game()->Now();
#ifdef __MOBILE
					notify->Notify("Tap here to spend achievement points!");
#else
					notify->Notify("Press U to spend achievement points!");
#endif
				}
			}
		}
    }
}
