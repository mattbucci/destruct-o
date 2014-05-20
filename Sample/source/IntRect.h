
#pragma once

class IntRect {
public:
	//Creates an all 0 int rect
	IntRect();
	//Constructors
	IntRect(float startX, float startY, float endX, float endY);
	IntRect(int startX, int startY, int endX, int endY);
	IntRect(pair<vec2,vec2> corners);

	//Handy Handy operators
	IntRect & operator+(int toAdd);
	IntRect & operator-(int toSub);
	IntRect & operator-(vec2 toSub);
	IntRect & operator*(int toMul);
	IntRect & operator/(int toDiv);
	IntRect & operator+=(int toAdd);
	IntRect & operator-=(int toSub);
	IntRect & operator-=(vec2 toSub);
	IntRect & operator*=(int toMul);
	IntRect & operator/=(int toDiv);

	//Modifies this intrect by intersecting it with another
	IntRect & Intersect(IntRect toIntersectWith);

	//Public members
	int StartX;
	int StartY;
	int EndX;
	int EndY;
};