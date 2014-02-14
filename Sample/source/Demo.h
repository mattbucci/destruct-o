
#pragma once

#include "stdafx.h"
#include "InputEvent.h"

class GL3DProgram;
class ParticleSystem;

class Demo {
	vector<ParticleSystem*> systems;

	void BuildCube(vec3 pos, float sepStrength);
public:

	Demo();

	void OnInput(vector<InputEvent> events, vec3 playerPos, vec3 playerFacing);

	void SwitchDemo(int newDemo, vec3 playerPos, vec3 playerFacing);

	void Update(double now, double delta);

	void Draw(GL3DProgram * shader);
};