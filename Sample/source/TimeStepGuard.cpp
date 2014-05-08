#include "stdafx.h"
#include "TimeStepGuard.h"
#include "TimeStepTracker.h"
#include "OS.h"

//Apply to the given tracker
//only one guard should ever apply to one tracker
//for consistent results
TimeStepGuard::TimeStepGuard(TimeStepTracker * tracker) {
	_ASSERTE(tracker != NULL);

	this->tracker = tracker;
	startTime = OS::Now();
}
TimeStepGuard::~TimeStepGuard() {
	//Add a recommendation now
	tracker->addRecommendation(OS::Now()-startTime);
}