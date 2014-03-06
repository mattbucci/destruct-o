#pragma once
#include "stdafx.h"

class Rect {
public:
	Rect();
	Rect(float width, float height);
	Rect(float x, float y, float width, float height);

	float X;
	float Y;
	float Width;
	float Height;

	//Checks if the given XY point is in the rect
	bool InRect(vec2 point);
	//Checks if the given XY point is in the rect if the
	//rect was at 0,0
	bool InWidthHeight(vec2 point);

	//Check if the given rect intersects with this rect
	bool Intersects(Rect other);

	bool operator==(const Rect & other);
	bool operator!=(const Rect & other);
};