#pragma once

#include "MovingAverage.h"

class TimeStepGuard;

//The time step tracker is used in conjunction with the TimeStepGuard
//to track the length of time spent in functions
//if the time is shorter than timeStepMinGoal
//a recommendation >1 is given
//if the time is shorter than timeStepMaxGoal
//a recommendation <1 is given
//recommendations go from .5-2 (50% to 200%)
//and are smoothed over 5 recommendations
class TimeStepTracker {
	//The shortest goal timestep
	double timeStepMinGoal;
	//the longest acceptable timestep
	double timeStepMaxGoal;
	//used to smooth recommendations
	MovingAverage<double> recommendationAverage;

	//Add a recommendation to the average based off of 
	//the amount of time it took to do whatever it is this tracker is tracking
	//added by a TimeStepGuard
	void addRecommendation(double timeStepTook);

	friend class TimeStepGuard;
public:
	TimeStepTracker(double minGoal, double maxGoal);

	//Get the recommendation from .5 to 2
	// <1 indicates a speed up is required (decrease detail of render/update)
	// >1 indicates a slow down is ok (increase detail of render/update)
	double GetRecommendation();
};