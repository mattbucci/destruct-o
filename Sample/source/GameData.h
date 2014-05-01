#pragma once

#include "stdafx.h"

#include "Options.h"

class GameData : public Savable {
public:

	Options GameOptions;

	CLASS_DECLARATION(Options)
		CLASS_MEMBER(GameOptions,ReflectionData::SAVE_INSTANCE)
	END_DECLARATION
};