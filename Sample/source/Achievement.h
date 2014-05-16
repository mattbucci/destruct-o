//
//  Achievement.h
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//

#ifndef __VoxEngine__Achievement__
#define __VoxEngine__Achievement__

#include <iostream>
#include <string>
#include "Achievements.h"

using namespace std;

class Achievement{
public:

    string type;
    float targetvalue;
    string displaytext;
    Achievements::AchievementType uniqueAchievementId;
	int rewardPoints;
    
    Achievement();
    Achievement(Achievements::AchievementType uniqueAchievementId, int reward, float targetvalue, string displaytext, string type);


};
#endif /* defined(__VoxEngine__Achievement__) */
