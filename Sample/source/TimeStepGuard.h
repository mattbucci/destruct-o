#pragma once

class TimeStepTracker;

//Track the time of an individual function
class TimeStepGuard {
	double startTime;
	TimeStepTracker * tracker;
public:
	//Apply to the given tracker
	//only one guard should ever apply to one tracker
	//for consistent results
	TimeStepGuard(TimeStepTracker * tracker);
	~TimeStepGuard();


};