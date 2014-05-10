#include "stdafx.h"
#include "GameData.h"
#include "OS.h"
#include "lodepng.h"

void GameData::Save() {
	static string saveDir = OS::SaveDirectory() + "Destructo_Saves/";
	//Save this class
	//Create relevant directories
	OS::BuildPath(saveDir);

	//Serialize this class (always allow the setting file to be hand editable)
	vector<unsigned char> saveData = Savable::Serialize(this,false);
	//Save the file
	lodepng::save_file(saveData,saveDir + "settings.json");
}
void GameData::Load() {
	static string saveDir = OS::SaveDirectory() + "Destructo_Saves/";
	//Load this class
	vector<unsigned char> saveData;

	//If there's no settings file the defaults will be used instead
	if (saveData.size() <= 0)
		return;

	lodepng::load_file(saveData,saveDir + "settings.json");
	//Deserialize this class (always allow the setting file to be hand editable)
	Savable::Deserialize(saveData,this,false);
}