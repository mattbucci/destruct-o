//
//  Effect.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//
#include "Effect.h"

Effect::Effect() {
    load_files();
    event test;
    test.type="gunshot";
    test.id=5;
    //PlayEffect(test);
}

//void Effect::PlayEffect(event action) {
 //   Mix_PlayChannel(1,effects[action.type],-1);
//}
bool Effect::load_files() {
    //Load the music
    fstream File("/sounds/Effects.txt");
    string tmp;
    //Effect File Defintion Format
        //EffectName pathtofile
    while(File) {
        //super ineffecient but garuntees broken lines won't mess up the rest
        getline(File,tmp);
        stringstream ss(tmp);
        vector<string> params;
        
        while (ss >> tmp) {
            params.push_back(tmp);
        }
        
        //ignore comment lines
        if(params[0].substr(0,2) != "//" && params.size() == 2) {
            effects.insert ( pair<string,Mix_Chunk*>(params[0],Mix_LoadWAV(params[1].c_str())) );
        }
        params.clear();
    }
    File.close();
    
    
    return true;
}
void Effect::SetVolume(uint8_t volume) {
    this->volume = volume;
    
    //sets unallocated channels to new volume
    Mix_Volume(-1, volume);
    
    //need to add a loop to adjust allocated channels
}