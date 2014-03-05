#pragma once

#include "stdafx.h"

class GLAcidTab {
	GLint timeUniform;
	GLint factorUniform;
public:
	GLAcidTab(GLint timeUniform, GLint factorUniform);
	~GLAcidTab();
	
	//The current update cycle time would work nicely here
	void SetCurrentTime(double time);
	//The amount the acid effects should be felt by the player
	//0 means none
	//1 means all
	void SetAcidFactor(float factor);
};