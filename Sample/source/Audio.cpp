//
//  Audio.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#include "Audio.h"


AudioPlayer::AudioPlayer(Uint8 volume) {
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(250);
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
void AudioPlayer::Subscribe(ActorPlayer* user) {
    
    Subscribe<void(ActorPlayer*,Effect*)>(user->PlayerJumped,[user,EffectPlayer](ActorPlayer* Object, Effect* EffectPlayer) {
            event test;
            test.type="ui-newability";
            test.id=5;
            EffectPlayer->PlayEffect(test);
	});
}