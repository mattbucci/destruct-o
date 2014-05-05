#include "stdafx.h"
#include "TimeStepTracker.h"

TimeStepTracker::TimeStepTracker(float minGoal, float maxGoal) : recommendationAverage(5) {
	timeStepMinGoal = .5f;
	timeStepMaxGoal = 2.0f;
}

//Add a recommendation to the average based off of 
//the amount of time it took to do whatever it is this tracker is tracking
//added by a TimeStepGuard
void TimeStepTracker::addRecommendation(float timeStepTook) {
	float recommendation = 1.0f;
	//If the frames are really fast, recommend higher detail
	if (timeStepTook < timeStepMinGoal) {
		recommendation = timeStepMinGoal/timeStepTook;
		if (recommendation > 2.0f)
			recommendation = 2.0f;
		//Blend higher detail recommendations
		//with the current recommendation
		//so it takes twice as long to increase as it does to decrease
		if (recommendationAverage.GetAverage() > 0)
			recommendation = (recommendationAverage.GetAverage()+recommendation)/2.0f;
	}
	//If the frames are really slow, recommend lower detail
	if (timeStepTook > timeStepMaxGoal) {
		recommendation = timeStepMaxGoal/timeStepTook;
		if (recommendation < .5f)
			recommendation = .5f;
	}

	//Add the recommendation
	recommendationAverage.AddSample(recommendation);
}

//Get the recommendation from .5 to 2
// <1 indicates a speed up is required (decrease detail of render/update)
// >1 indicates a slow down is ok (increase detail of render/update)
float TimeStepTracker::GetRecommendation() {
	return recommendationAverage.GetAverage();
}