/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ACHIEVEMENT_PROGRESS_H__
#define __ACHIEVEMENT_PROGRESS_H__

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

#endif
