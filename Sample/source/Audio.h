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
#include "Demo.h"

class AudioPlayer : protected GameEventSubscriber {
private:
    Music MusicPlayer;
    Effect EffectPlayer;
    void clean_up();
public:
    AudioPlayer(uint8_t volume);
    void Pause();
    void Resume();
    void SetVolume(uint8_t effectvolume, uint8_t backgroundvolume);
    void SetIntensity(uint8_t value);
    void ReceiveEvent(event action);
    void PlayerInit(ActorPlayer*);
    void DemoInit(Demo* demo);
};
#endif /* defined(__Audio_Test__Audio__) */
