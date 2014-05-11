#pragma once

#include "RootSavable.h"

#include "Achievements.h"

//Per-account data
//might also be a good place to store things like
//cached security tokens, or usernames or whatever
class AccountData : public RootSavable {
public:
	AccountData();

	Achievements GameAchievements;

	CLASS_DECLARATION(AccountData)
		INHERITS_FROM(RootSavable)
		CLASS_MEMBER(GameAchievements,ReflectionData::SAVE_INSTANCE)
	END_DECLARATION
};