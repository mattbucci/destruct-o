//
//  Music.h
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#ifndef __Audio_Test__Music__
#define __Audio_Test__Music__

#include "AudioHeader.h"

class Music {
private:
    uint8_t volume;
    uint8_t sceneintensity;
    struct track{
        uint8_t intensity;
        string trackname;
        string filename;
    };
    
    //general music for bossfights, menus, etc
    map<string,track> tracks;
    //music played randomly, scrambled on generation and repeat, played sequentially
    vector< vector<track> > ambient_tracks;
    vector<int> track_numbers;
    
    void play_track(string song);
    void play_track(track song);
    bool load_tracks();
    bool shouldbeplaying;
    void start_playback();
    
    Mix_Music *music;
public:
    Music();
    ~Music();
    void SetVolume(uint8_t volume);
    void Update();
    void Pause();
    void Resume();
};

#endif /* defined(__Audio_Test__Music__) */
