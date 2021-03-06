//
//  Audio.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef __Audio_Test__Audio__
#define __Audio_Test__Audio__

#include "AudioHeader.h"
#include "Music.h"
#include "Effect.h"
#include "ActorPlayer.h"

class AudioPlayer : protected GameEventSubscriber {
private:
    ActorPlayer * user;
    Music MusicPlayer;
    Effect EffectPlayer;
public:
    AudioPlayer(uint8_t volume);
    void Pause();
    void Update();
    void Resume();
    void SetVolume(uint8_t effectvolume, uint8_t backgroundvolume);
    void SetIntensity(uint8_t value);
};
#endif /* defined(__Audio_Test__Audio__) */
