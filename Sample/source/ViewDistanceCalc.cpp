#include "stdafx.h"
#include "ViewDistanceCalc.h"
#include "GL3DProgram.h"


ViewDistanceCalc::ViewDistanceCalc() {
	viewDistance = 0.0;
	framesDrawn = 0;
}

//Should be called every draw cycle
//calculates the view distance
//applies to fog distance automatically
void ViewDistanceCalc::CalculateAndApply(GL3DProgram * shader,float currentFPS) {
	//Constants used for adjustment
	//How long the visual learning rate is
	const static int maxFrameLearn = 400;
	//Target frame rates
	const float maxFrameRate = 45;
	//This should be a bit higher than what you actually want
	const float acceptableFrameRate = 35;
	framesDrawn++;
	//only calibrate for the first 2000 frames
	if (framesDrawn > maxFrameLearn)
		return;

	//A value between 0 and 1 which indicates how fast
	//the view distance can be adjusted
	float confidenceFactor = framesDrawn/(float)maxFrameLearn;
	//confidenceFactor^3 is used to slow the learning rate very early on
	confidenceFactor = 1-confidenceFactor;
	confidenceFactor = confidenceFactor*confidenceFactor*confidenceFactor;
	

	//If lower than the acceptable framerate, lower the view distance quickly
	if (currentFPS < acceptableFrameRate) 
		viewDistance -= confidenceFactor*(acceptableFrameRate-currentFPS)/acceptableFrameRate*.05f;
	else if (currentFPS > maxFrameRate) 
		viewDistance += confidenceFactor*(currentFPS-acceptableFrameRate)/acceptableFrameRate*.02f;
	
	if (viewDistance < 0.0)
		viewDistance = 0.0;
	if (viewDistance > 1.0)
		viewDistance = 1.0;

	//Fog has to end before view distance
	//because view distance is not perfect
	shader->Fog.SetFogDistance(GetViewDistance()*.80f);
}

//Retrieve a pair of coordinates representing the appropriate draw section
pair<vec2,vec2> ViewDistanceCalc::VoxDrawCoordinates(vec2 viewPortSize, vec2 mapExtents, vec2 userPosition, float userAngle) {
	//New strategy for determining draw box
	//Create a rectangle with the user on the bottom center, with the top of the rectangle
	//far away from the user in his view direction
	//Use the points of this rotated rectangle
	//to choose a min/max point of the voxel draw rectangle

	//Width code is highly experimental and will certainly have to be played with
	float rectHeight = GetViewDistance(); //the full length of the rectangle
	float rectHalfWidth = pow(rectHeight/50.0f,2.0f)+rectHeight/5.0f+10.0f; //Half the width of the rectangle
	float rectHalfDiagonal =(float)( M_PI/2.0f-atan2(rectHeight,rectHalfWidth)); //The angle of the diagonal (to the center of the width side)
	float rectDiagonalLength = sqrt(rectHalfWidth*rectHalfWidth+rectHeight*rectHeight);
	vec2 testPoints[4] = {
		//Use the edges of a rectangle with the viewer in the bottom center
		//First the bottom left
		userPosition + vec2(cos(M_PI/2.0f+userAngle),sin(M_PI/2.0f+userAngle))*rectHalfWidth,
		//Next bottom right
		userPosition + vec2(cos(-M_PI/2.0f+userAngle),sin(-M_PI/2.0f+userAngle))*rectHalfWidth,
		//Top Left
		userPosition + vec2(cos(rectHalfDiagonal+userAngle),sin(rectHalfDiagonal+userAngle))*rectDiagonalLength,
		//Top Right
		userPosition + vec2(cos(-rectHalfDiagonal+userAngle),sin(-rectHalfDiagonal+userAngle))*rectDiagonalLength,
	};
	vec2 minPoint = mapExtents;
	vec2 maxPoint = vec2(0,0);
	for (int i = 0; i < 4; i++) {
		minPoint = glm::min(minPoint,testPoints[i]);
		maxPoint = glm::max(maxPoint,testPoints[i]);
	}

	minPoint = glm::floor(minPoint);
	maxPoint = glm::ceil(maxPoint);
	//Limit points to valid ranges (vec2() creates a zero vector)
	minPoint = glm::max(vec2(),minPoint);
	maxPoint = glm::min(mapExtents-vec2(1,1),maxPoint);
	return pair<vec2,vec2>(minPoint,maxPoint);
}

float ViewDistanceCalc::GetViewDistance() {
	//viewDistance is between 0 to 1, so apply scale now
	return viewDistance*(MAX_DRAW_DISTANCE-MIN_DRAW_DISTANCE)+MIN_DRAW_DISTANCE;
}