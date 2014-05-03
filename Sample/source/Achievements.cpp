//
//  Achievements.cpp
//  VoxEngine
//
//  Created by Matthew Bucci on 4/8/14.
//
//
// Include the potentially precompiled header
#include "stdafx.h"

#include "BaseFrame.h"
#include "ActorPlayer.h"
#include "Achievements.h"
#include "Achievement.h"


Achievements::Achievements(Notification* notification, BaseFrame * game){
    this->notify = notification;
    achievementlist.push_back(new Achievement(0,5,"Baby Steps, 5 steps taken","player-walked",0,notify));
    achievementlist.push_back(new Achievement(0,25,"One Small Step for man, 25 steps taken","player-walked",0,notify));
    achievementlist.push_back(new Achievement(0,100,"Where's the Fire?, 100 steps taken","player-walked",0,notify));
    
    achievementlist.push_back(new Achievement(0,1,"Learning to fly, Jumped for the first time","player-jumped",0,notify));
    
    achievementlist.push_back(new Achievement(0,100,"Destroyer of worlds, 100 voxels destroyed","voxel-destroyed",0,notify));
    

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
};

void Achievements::incrementAchievement(string name, float incrementQuantity) {
    for(unsigned int i = 0; i < this->achievementlist.size(); i++) {
        if(achievementlist[i]->type==name)
            achievementlist[i]->IncrementValue(incrementQuantity);
    }
}