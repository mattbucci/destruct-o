//
//  Music.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef __Audio_Test__Music__
#define __Audio_Test__Music__
#include "Audio.h"
#include <fstream>

class Music {
private:
    uint8_t volume = 100;
    struct track{
        uint8_t intensity;
        std::string trackname;
        std::string filename;
    };
    //general music for bossfights, menus, etc
    std::map<std::string,track> tracks;
    //music played randomly, scrambled on generation and repeat, played sequentially
    std::vector<track> ambient_tracks;
    int sceneintensity = 0;
    void play_track(std::string song);
    bool load_tracks();
    Mix_Music *music;
public:
    Music();
    void SetVolume(uint8_t volume);
    void Pause();
    void Resume();
};

#endif /* defined(__Audio_Test__Music__) */
