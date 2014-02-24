//
//  Music.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#include "Music.h"


Music::Music() {
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    cout << "Loading music" << endl;
    load_tracks();
    play_track("menu");
}

Music::~Music() {
    while(!Mix_FadeOutMusic(3000) && Mix_PlayingMusic()) {
        // wait for any fades to complete
        SDL_Delay(100);
    }
}

void Music::start_playback() {
    
    
}

bool Music::load_tracks() {
    //Load the music
    fstream File("sounds/Music.txt");
    string tmp;
    //Music File Defintion Format
    //TrackName Intensity pathtofile
    //Special Tracks are marked with intensity 0
    
    while(File) {
        //super ineffecient but garuntees broken lines won't mess up the rest
        getline(File,tmp);
        if(tmp != "\0" && tmp != "\n") {
            stringstream ss(tmp);
            vector<string> params;
            
            while (ss >> tmp) {
                params.push_back(tmp);
            }
            
            //ignore comment lines
            if(params[0].substr(0,2) != "//" && params.size() == 3) {
                //special tracks marked with intensity 0
                if(params[1] == "0")
                {
                    track special;
                    special.intensity = 0;
                    special.trackname = params[0];
                    special.filename = params[2];
                    tracks.insert(std::pair<std::string,track>(params[0],special));
                }
                //ambient tracks
                else {
                    track ambient;
                    ambient.intensity = atoi(params[0].c_str());
                    ambient.trackname = params[0];
                    ambient.filename = params[2];
                    ambient_tracks.push_back(ambient);
                }
            }
            params.clear();
        }
        tmp = '\0';
    }
    File.close();
    
    
    return true;
}

void Music::play_track(std::string song) {
    
    //finds whether or not the track exists without instantiating it like [] would
    map<std::string,track>::iterator it = tracks.find(song);
    track playing;
    if(it != tracks.end())
    {
        //if track exists
        playing = it->second;
        music = Mix_LoadMUS(playing.filename.c_str());
        if(music)
            Mix_PlayMusic(music, 1);
        
    }
}
void Music::SetVolume(uint8_t volume) {
    this->volume = volume;
    Mix_VolumeMusic(volume);
}