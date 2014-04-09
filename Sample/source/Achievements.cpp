//
//  Achievements.cpp
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//
// Include the potentially precompiled header
#include "stdafx.h"

#include "Achievements.h"

Achievements::Achievements(Notification* notification){
    this->notify = notification;
    achievementlist.push_back(new Achievement(0,5,"Baby Steps, 5 steps taken","player-walked",0,notify));
    achievementlist.push_back(new Achievement(0,25,"One Small Step for man, 25 steps taken","player-walked",0,notify));
    achievementlist.push_back(new Achievement(0,100,"Where's the Fire?, 100 steps taken","player-walked",0,notify));
    
    achievementlist.push_back(new Achievement(0,1,"Learning to fly, Jumped for the first time","player-jumped",0,notify));
    
    achievementlist.push_back(new Achievement(0,1,"Destroyer of worlds, 100 voxels destroyed","voxel-destroyed",0,notify));
    
};

void Achievements::InitPlayerAchievements(ActorPlayer * player){
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerJumped,[this](ActorPlayer* Object) {
        for(int i = 0; i < this->achievementlist.size(); i++) {
            if(achievementlist[i]->type=="player-jumped")
                achievementlist[i]->IncrementValue(1);
        }
    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerLanded,[this](ActorPlayer* Object) {
        for(int i = 0; i < this->achievementlist.size(); i++) {
            if(achievementlist[i]->type=="player-landed")
                achievementlist[i]->IncrementValue(1);
        }
    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerWalked,[this](ActorPlayer* Object) {
        for(int i = 0; i < this->achievementlist.size(); i++) {
            if(achievementlist[i]->type=="player-walked")
                achievementlist[i]->IncrementValue(1);
        }
    });

}

void Achievements::InitPhysicsAchievements(PhysicsSystem * physics){
    GameEventSubscriber::Subscribe<void(PhysicsVoxel*)>(&physics->VoxelDisintegrating,[this](PhysicsVoxel* Object) {
        for(int i = 0; i < this->achievementlist.size(); i++) {
            if(achievementlist[i]->type=="voxel-destroyed")
                achievementlist[i]->IncrementValue(1);
        }
    });
}

void Achievements::InitActorAchievements(ActorSystem * actors){
    
}
