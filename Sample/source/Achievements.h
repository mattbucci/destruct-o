//
//  Achievements.h
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//

#ifndef __VoxEngine__Achievements__
#define __VoxEngine__Achievements__

#include <iostream>
#include <vector>
#include "Achievement.h"
#include "PhysicsSystem.h"
#include "ActorPlayer.h"
#include "ActorSystem.h"
#include "Notification.h"

class Achievements : protected GameEventSubscriber{
    vector<Achievement*> achievementlist;
    Notification * notify;
public:
    Achievements(){};
    Achievements(Notification* notification){
        this->notify = notification;
    };
    
    void InitPlayerAchievements(ActorPlayer * player);
    
    void InitPhysicsAchievements(PhysicsSystem * physics);
    
    void InitActorAchievements(ActorSystem * actors);
    
    GameEvent<void(Achievement*)> AchivementEarned;
};

#endif /* defined(__VoxEngine__Achievements__) */
