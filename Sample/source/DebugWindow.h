
#pragma once

#include "Window.h"

class Label;

class DebugWindow : public Window {
	Label * fps;
	Label * voxels;
	Label * drawTime;
	Label * updateTime;
	unsigned int curDrawCycle;
public:
	DebugWindow();
	
	void Draw(GL2DProgram * shader) override;

	//Information automatically displayed during draw for the debug window
	float Fps;
	int Voxels;
	float DrawTime;
	float UpdateTime;
};