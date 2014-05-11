#pragma once

#include "stdafx.h"

#include "RootSavable.h"
#include "Options.h"

class DeviceData : public RootSavable {
public:
	DeviceData();

	//Settings and such for the game
	Options GameOptions;

	CLASS_DECLARATION(DeviceData)
		INHERITS_FROM(RootSavable)
		CLASS_MEMBER(GameOptions,ReflectionData::SAVE_INSTANCE)
	END_DECLARATION
};