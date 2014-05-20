
#pragma once

#include "Achievements.h"

//Stores the progress for each achievement
class AchievementProgress : public Savable{
public:
	//Used by save/load
	AchievementProgress();
	//Create a progress tracker for a specific achievement
	AchievementProgress(Achievements::AchievementType uniqueId);
	~AchievementProgress();

	//the data associated with this achievement
	Achievements::AchievementType UniqueId;
	//The total progress
	float Progress;
	//Whether or not the user has earned this achievement
	bool Earned;

	CLASS_DECLARATION(AchievementProgress)
		CLASS_MEMBER(UniqueId,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(Progress,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(Earned,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};