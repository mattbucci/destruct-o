#include "stdafx.h"
#include "IntRect.h"

//Creates an all 0 int rect
IntRect::IntRect() {
	StartX = StartY = EndX = EndY = 0;
}
//Constructors
IntRect::IntRect(float startX, float startY, float endX, float endY) {
	StartX = (int)startX;
	StartY = (int)startY;
	EndX = (int)endX;
	EndY = (int)endY;
}
IntRect::IntRect(int startX, int startY, int endX, int endY) {
	StartX = startX;
	StartY = startY;
	EndX = endX;
	EndY = endY;
}
IntRect::IntRect(pair<vec2,vec2> corners) {
	StartX = (int)corners.first.x;
	StartY = (int)corners.first.y;
	EndX = (int)corners.second.x;
	EndY = (int)corners.second.y;	
}

//Handy Handy operators
IntRect & IntRect::operator+(int toAdd) {
	return *this += toAdd;
}
IntRect & IntRect::operator-(int toSub) {
	return *this -= toSub;
}
IntRect & IntRect::operator-(vec2 toSub) {
	return *this -= toSub;
}
IntRect & IntRect::operator*(int toMul) {
	return *this *= toMul;
}
IntRect & IntRect::operator/(int toDiv) {
	return *this /= toDiv;
}
IntRect & IntRect::operator+=(int toAdd) {
	StartX += toAdd;
	StartY += toAdd;
	EndX += toAdd;
	EndY += toAdd;
	return *this;
}
IntRect & IntRect::operator-=(int toSub) {
	StartX -= toSub;
	StartY -= toSub;
	EndX -= toSub;
	EndY -= toSub;
	return *this;
}
IntRect & IntRect::operator-=(vec2 toSub) {
	StartX -= (int)toSub.x;
	StartY -= (int)toSub.y;
	EndX -= (int)toSub.x;
	EndY -= (int)toSub.y;
	return *this;
}
IntRect & IntRect::operator*=(int toMul) {
	StartX *= toMul;
	StartY *= toMul;
	EndX *= toMul;
	EndY *= toMul;
	return *this;
}
IntRect & IntRect::operator/=(int toDiv) {
	StartX /= toDiv;
	StartY /= toDiv;
	EndX /= toDiv;
	EndY /= toDiv;
	return *this;
}


//intersects two ranges
//places the output into rangeA
static void intersect1D(int & rangeAStart, int & rangeAEnd, int rangeBStart, int rangeBEnd) {
	//No intersection
	if ((rangeAStart > rangeBEnd) || (rangeAEnd < rangeBStart)) {
		rangeAEnd = rangeAStart;
		return;
	}
	//Find the range intersection
	rangeAStart = max(rangeAStart, rangeBStart);
	rangeAEnd = min(rangeAEnd, rangeBEnd);
}


//Modifies this intrect by intersecting it with another
IntRect & IntRect::Intersect(IntRect toIntersectWith) {
	intersect1D(StartX,EndX,toIntersectWith.StartX,toIntersectWith.EndX);
	intersect1D(StartY,EndY,toIntersectWith.StartY,toIntersectWith.EndY);

	return *this;
}
