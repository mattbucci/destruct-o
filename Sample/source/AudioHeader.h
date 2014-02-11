//
//  AudioHeader.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/10/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef Audio_Test_AudioHeader_h
#define Audio_Test_AudioHeader_h
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <list>

using namespace std;

//sudo event, to be overridden by anthony's code during production
struct event{
    uint32_t id;
    string type;
};

#endif
