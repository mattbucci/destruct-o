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

void Achievements::InitPlayerAchievements(ActorPlayer * player){
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerJumped,[this](ActorPlayer* Object) {

    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerLanded,[this](ActorPlayer* Object) {

    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&player->PlayerWalked,[this](ActorPlayer* Object) {

    });

}

void Achievements::InitPhysicsAchievements(PhysicsSystem * physics){
    
}

void Achievements::InitActorAchievements(ActorSystem * actors){
    
}