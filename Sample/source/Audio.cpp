//
//  Audio.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#include "stdafx.h"
#include "Audio.h"


AudioPlayer::AudioPlayer(Uint8 volume) {
}

void AudioPlayer::SetVolume(Uint8 effectvolume,Uint8 backgroundvolume) {
   // MusicPlayer.SetVolume();
   // EffectPlayer.SetVolume();
}

void AudioPlayer::ReceiveEvent(event action) {
    //check the event type
    //if player paused, pause music and effect
    
    //if effect
    //pass the event to the effect handler
    
    //if in battle
    //set the intensity
}
void AudioPlayer::PlayerInit(ActorPlayer* user) {
    this->user = user;
	//FIXME
	/*
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&user->PlayerJumped,[this](ActorPlayer* Object) {
        event test;
        test.type="player-jump";
        test.id=1;
        test.pos = vec3(0,0,0);
        EffectPlayer.PlayEffect(test);
    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&user->PlayerLanded,[this](ActorPlayer* Object) {
        event test;
        test.type="player-land";
        test.id=2;
        test.pos = vec3(0,0,0);
        EffectPlayer.PlayEffect(test);
    });
    
    GameEventSubscriber::Subscribe<void(ActorPlayer*)>(&user->PlayerWalked,[this](ActorPlayer* Object) {
        event test;
        test.type="player-walk";
        test.id=3;
        test.pos = vec3(0,0,0);
        EffectPlayer.PlayEffect(test);
    });
	*/

}

void AudioPlayer::DemoInit(Demo* demo) {
    GameEventSubscriber::Subscribe<void(vec3)>(&demo->CraterCalled,[this](vec3 Position) {
        event test;
        test.type="explosions-grenade";
        test.id=4 + rand() % 50;
        test.pos = Position - user->GetPosition();
        EffectPlayer.PlayEffect(test);
    });
}

void AudioPlayer::Update(){
    MusicPlayer.Update();
}