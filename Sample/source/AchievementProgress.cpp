#include "stdafx.h"
#include "AchievementProgress.h"

CLASS_SAVE_CONSTRUCTOR(AchievementProgress)

AchievementProgress::AchievementProgress() {
	//Load should follow
}

AchievementProgress::AchievementProgress(Achievements::AchievementType uniqueId) {
	this->UniqueId = uniqueId;
	this->Progress = 0;
	Earned = false;
}
AchievementProgress::~AchievementProgress() {

}