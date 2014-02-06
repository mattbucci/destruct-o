//
//  Audio.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef __Audio_Test__Audio__
#define __Audio_Test__Audio__
#include <SDL2_mixer/SDL_mixer.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "Music.h"
#include "Effect.h"


//sudo event, to be overridden by anthony's code during production
struct event{
    uint32_t id;
    std::string type;
};


class AudioPlayer {
private:
    //Music MusicPlayer;
    //Effect EffectPlayer;
    void clean_up();
public:
    AudioPlayer(uint8_t volume);
    void Pause();
    void Resume();
    void SetVolume(uint8_t effectvolume, uint8_t backgroundvolume);
    void SetIntensity(uint8_t value);
    void ReceiveEvent(event action);
};
#endif /* defined(__Audio_Test__Audio__) */
