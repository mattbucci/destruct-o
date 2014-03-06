#pragma once

#include "stdafx.h"

class FPSCounter {
	list<double> frameTimes;
	void cleanupOldFrames();
public:
	void MarkFrameRendered();
	float GetFps();
}; 
/*



*/