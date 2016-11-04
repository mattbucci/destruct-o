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

#ifndef __ACTOR_AI_BOMBER_H__
#define __ACTOR_AI_BOMBER_H__

#include "ActorAI.h"
#include "EffectLaser.h"

class Weapon;

class ActorAIBomber : public ActorAI {
	bool runStarted;
	vec2 runStartPosition;
	vec2 runDirection;
	int turnarounddistance;
protected:
	//Bombers don't find a target and then attack it
	//they find a target and then set up a bombing run

	//Path until you're in range of the bomb run
	virtual void stateEngaging(bool & holdingTrigger);


public:
	ActorAIBomber();
	~ActorAIBomber();

	//The simple things, such as moving
	//or facing
	//should go here
	//runs at 100hz
	virtual void cheapUpdate();


	//Whether or not a bombing run completed
	bool BombingRunComplete();

	CLASS_DECLARATION(ActorAIBomber)
		INHERITS_FROM(ActorAI)
		CLASS_MEMBER(runStarted,ReflectionData::SAVE_BOOL);
		CLASS_MEMBER(runStartPosition,ReflectionData::SAVE_VEC2);
		CLASS_MEMBER(runDirection,ReflectionData::SAVE_VEC2);
	END_DECLARATION
};

#endif
