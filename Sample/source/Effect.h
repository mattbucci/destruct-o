//
//  Effect.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef __Audio_Test__Effect__
#define __Audio_Test__Effect__
#include "AudioHeader.h"


class Effect{
private:
    uint8_t volume = 100;
    map<string,Mix_Chunk*> effects;
    map<uint32_t,uint16_t> channels;
    bool load_files();
public:
    Effect();
    void SetVolume(uint8_t volume);
    //void PlayEffect(event action);
    void Pause();
    void Resume();
    void Play();
};

#endif /* defined(__Audio_Test__Effect__) */
