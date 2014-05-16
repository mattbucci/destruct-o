#pragma once

#include "stdafx.h"

class RootSavable : public Savable {
	string saveName;
	bool compressed;
public:
	//Set properties for Save() Load()
	RootSavable(string saveNmae, bool compressed);

	void Save();
	void Load();

	CLASS_DECLARATION(RootSavable)
	END_DECLARATION
};