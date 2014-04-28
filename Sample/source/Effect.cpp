//
//  Effect.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#include "stdafx.h"
#include "Effect.h"

Effect::Effect() {
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(255);
    cout << "Loading effects" << endl;
    load_files();
    event test;
    test.type="ui-newability";
    test.id=5;
    PlayEffect(test);
}

void Effect::PlayEffect(event action) {
    if(action.type=="player-walk"){
        if(foot)
            Mix_PlayChannel(action.id,effects["player-left-foot"],0);
        else
            Mix_PlayChannel(action.id,effects["player-right-foot"],0);
    }
    else {
        if (action.pos != vec3(0,0,0)) {
			float distancefactor = 3*(pow(glm::length(action.pos - vec3(0, 0, 0)), -.3f) - .2f);
			if (distancefactor > 1) distancefactor = 1;
			cout << distancefactor << endl;
            int distance = (int)(255 - 255 * distancefactor);

			//Seems a little strange, cmath functions take radians and return them
			//but 180 is degrees typically
            int angle = (int)(acos(action.pos.x)+(action.pos.y < 0 ? 180 : 0));
            
            Mix_SetPosition(action.id, angle, distance);
            
        }
        Mix_PlayChannel(action.id,effects[action.type],0);
    }
}


void ReadFiles(string data, map<string,Mix_Chunk*>& effects) {

    
}

bool Effect::load_files() {
    //Load the effects
    string filename = "sounds/Effects.txt";
    
	SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r");
	long size;
    
	//Use the SDL system to read the file
	SDL_RWseek(file , 0 , RW_SEEK_END);
	size = (long)SDL_RWtell(file);
	SDL_RWseek(file,0,RW_SEEK_SET);
    
	char * fileData = new char[size];
	SDL_RWread(file,fileData, 1, (size_t)size);
	SDL_RWclose(file);
    
    stringstream File(string(fileData,size));
    
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
        if(params.size() == 2) {
            if(params[0].substr(0,2) != "//") {
                cout << "Loaded: " << params[0]<< endl;
                effects.insert ( pair<string,Mix_Chunk*>(params[0],Mix_LoadWAV(params[1].c_str())) );
            }
        }
        params.clear();
    }

    
    
    return true;
}

void Effect::SetVolume(uint8_t volume) {
    this->volume = volume;
    
    //sets unallocated channels to new volume
    Mix_Volume(-1, volume);
    
    //need to add a loop to adjust allocated channels
}