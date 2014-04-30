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


/*
void AudioPlayer::PlayerInit(ActorPlayer* user) 
 {
    this->user = user;
 
	//FIXME
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
}
*/

void AudioPlayer::Update(){
    MusicPlayer.Update();
}

void AudioPlayer::Pause()
{
    MusicPlayer.Pause();
}

void AudioPlayer::Resume()
{
    MusicPlayer.Resume();
}
