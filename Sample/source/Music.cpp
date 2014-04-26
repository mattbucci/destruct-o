//
//  Music.cpp
//  Audio Test
//
//  Created by Matthew Bucci on 2/6/14.
//  Copyright (c) 2014 Matthew Bucci. All rights reserved.
//

#include "stdafx.h"
#include "Music.h"


Music::Music() {
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    
    cout << "Loading music" << endl;
    sceneintensity = 0;
    shouldbeplaying = false;
    
    ambient_tracks.resize(10);
    track_numbers.resize(10);
    
    for(int i = 0; i < track_numbers.size(); i++){
        track_numbers[i] = 0;
    }
    
    load_tracks();
    start_playback();
}

Music::~Music() {
    while(!Mix_FadeOutMusic(3000) && Mix_PlayingMusic()) {
        // wait for any fades to complete
        SDL_Delay(100);
    }
}


void Music::start_playback() {
    shouldbeplaying = true;
    play_track(ambient_tracks[sceneintensity][track_numbers[sceneintensity]]);
}
bool Music::load_tracks() {
	//Load the effects
	string filename = "sounds/Music.txt";

	SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r");
	long size;

	//Use the SDL system to read the file
	SDL_RWseek(file, 0, RW_SEEK_END);
	size = (long)SDL_RWtell(file);
	SDL_RWseek(file, 0, RW_SEEK_SET);

	char * fileData = new char[size];
	SDL_RWread(file, fileData, 1, (size_t)size);
	SDL_RWclose(file);

	stringstream File(string(fileData, size));

	string tmp;
	//Effect File Defintion Format
	//EffectName pathtofile
	while (File) {
		//super ineffecient but garuntees broken lines won't mess up the rest
		getline(File, tmp);
		stringstream ss(tmp);
		vector<string> params;

		while (ss >> tmp) {
			params.push_back(tmp);
		}
		//ignore comment lines
		if (params.size() == 3 && params[0].substr(0, 2) != "//") {
			//special tracks marked with intensity 0
			if (params[1] == "-1")
			{
				track special;
				cout << "Loaded: " << params[0] << endl;
				special.intensity = 0;
				special.trackname = params[0];
				special.filename = params[2];
				tracks.insert(std::pair<std::string, track>(params[0], special));
			}
			//ambient tracks
			else {
				track ambient;
				cout << "Loaded: " << params[0] << endl;
				ambient.intensity = atoi(params[0].c_str());
				ambient.trackname = params[0];
				ambient.filename = params[2];
				ambient_tracks[ambient.intensity].push_back(ambient);
			}
		}
		params.clear();
	}



	return true;
}


void Music::play_track(std::string song) {
    //for playback of special tracks
	if (song != "") {
		//finds whether or not the track exists without instantiating it like [] would
		map<std::string, track>::iterator it = tracks.find(song);
		track playing;
		if (it != tracks.end())
		{
			//if track exists
			playing = it->second;
			music = Mix_LoadMUS(playing.filename.c_str());
			if (music)
				Mix_PlayMusic(music, 1);

		}
	}
}

void Music::play_track(track song) {
    //for playback of ambient tracks
    music = Mix_LoadMUS(song.filename.c_str());
    if (music) {
        Mix_PlayMusic(music, 1);
    }
}

void Music::SetVolume(uint8_t volume) {
    this->volume = volume;
    Mix_VolumeMusic(volume);
}

void Music::Update(){
    if(!Mix_PlayingMusic() && shouldbeplaying) {
        track_numbers[sceneintensity]++;
        if(ambient_tracks[sceneintensity].size() >= sceneintensity +1)
            play_track(ambient_tracks[sceneintensity][track_numbers[sceneintensity]]);
    }
}