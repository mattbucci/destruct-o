//
//  AudioHeader.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/10/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef Audio_Test_AudioHeader_h
#define Audio_Test_AudioHeader_h

// Include the potentially precompiled header
#include "stdafx.h"

// Include SDL2_mixer
#if (defined __MACOSX__)
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif

#include <sstream>
#include <fstream>

using namespace std;

//sudo event, to be overridden by anthony's code during production
struct event
{
    uint32_t id;
    string type;
};

#endif
