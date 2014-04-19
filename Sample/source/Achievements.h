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
class Notification;
class BaseFrame;

class Achievements : protected GameEventSubscriber{
    vector<Achievement*> achievementlist;
    Notification * notify;

	void incrementAchievement(string name, float incrementQuantity);
public:
    Achievements(Notification* notification, BaseFrame * game);
    
    void Update();
    
    GameEvent<void(Achievement*)> AchivementEarned;
};

#endif /* defined(__VoxEngine__Achievements__) */
