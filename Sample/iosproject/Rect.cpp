#include "stdafx.h"
#include "Rect.h"

Rect::Rect() {
	X=Y=Width=Height = 0;
}
Rect::Rect(float width, float height) {
	Width = width;
	Height = height;
	X=Y=0;
}
Rect::Rect(float x, float y, float width, float height) {
	Width = width;
	Height = height;
	X = x;
	Y = y;
}

bool Rect::Intersects(Rect other) {
	//Lets hope the compiler can clean this up for me
	//I'm too lazy to fix it myself
	float aRight = X+Width;
	float aTop = Y;
	float bRight = other.X + other.Width;
	float bTop = other.Y;
	float aLeft = X;
	float bLeft = other.X;
	float aBottom = Y+Height;
	float bBottom = other.Y + other.Height;
	return (aLeft <= bRight &&
			bLeft <= aRight &&
			aTop <= bBottom &&
			bTop <= aBottom);
}


//Checks if the given XY point is in the rect
bool Rect::InRect(vec2 point) {
	return ((X <= point.x) && (X+Width >= point.x)) && ((Y <= point.y) && (Y+Height >= point.y));
}
//Checks if the given XY point is in the rect if the
//rect was at 0,0
bool Rect::InWidthHeight(vec2 point) {
	return ((0 <= point.x) && (Width >= point.x)) && ((0 <= point.y) && (Height >= point.y));
}


bool Rect::operator==(const Rect & other) {
	return ((X==other.X)&&(Y==other.Y)&&(Width==other.Width)&&(Height==other.Height));
}
bool Rect::operator!=(const Rect & other) {
	return !(*this==other);
}