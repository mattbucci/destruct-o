
#pragma once

#include "stdafx.h"
#include "InputEvent.h"
#include "GameEvent.h"

class GL3DProgram;
class ParticleSystem;

class Demo {
	vector<ParticleSystem*> systems;

	void BuildCube(vec3 pos, float sepStrength);

	//-1 if no key press queued
	//or >1 if we want to fake a key press on next Update()
	int queuedKeyPress;

	vector<unsigned char> resetSave;
public:

	//saves the save the reset button loads
	void DoInitialSave();

	void QueueTouchEvent(int keyPress);

	Demo();

	void CheckTouchInput(vec3 playerPos, vec3 playerFacing);

	void OnInput(vector<InputEvent> events, vec3 playerPos, vec3 playerFacing);

	void SwitchDemo(int newDemo, vec3 playerPos, vec3 playerFacing);

	void Update();

	float CurrentAcidStrength;

	void Draw(GL3DProgram * shader);
    
    
    //duct tape fix
	GameEvent<void(vec3)> CraterCalled;
    GameEvent<void(vec3)> FireCalled;
};