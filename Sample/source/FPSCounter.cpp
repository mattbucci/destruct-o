#include "stdafx.h"
#include "FPSCounter.h"
#include "OS.h"

//Remove items from the list too old to be considered now
void FPSCounter::cleanupOldFrames() {
	double cutOff = OS::Now();
	cutOff -= 1;
	for (auto it = frameTimes.begin(); it != frameTimes.end(); ) {
		if (*it < cutOff)
			it = frameTimes.erase(it);
		else
			it++;
	}
}

void FPSCounter::MarkFrameRendered() {
	cleanupOldFrames();
	frameTimes.push_back(OS::Now());
}
float FPSCounter::GetFps() {
	cleanupOldFrames();
	return (float)frameTimes.size()/1.0f;
}