//
//  Achievement.cpp
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//
#include "stdafx.h"
#include <sstream>

#include "Achievement.h"

Achievement::Achievement(Achievements::AchievementType uniqueAchievementId, int reward, float targetvalue, string displaytext, string type){
    this->targetvalue =  targetvalue;
    this->displaytext = displaytext;
    this->type = type;
	this->uniqueAchievementId = uniqueAchievementId;
	this->rewardPoints = reward;
}

Achievement::Achievement() {
    targetvalue = 0;
    displaytext = "";
    type="Other";
	this->rewardPoints = -100;
}

