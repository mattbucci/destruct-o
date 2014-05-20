#pragma once

#include "stdafx.h"

class FPSCounter {
	vector<double> frameTimes;
	void cleanupOldFrames();
public:
	void MarkFrameRendered();
	float GetFps();
}; 
/*



*/