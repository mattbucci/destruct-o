#pragma once

#include "stdafx.h"

#include <sstream>

class Utilities {
public:
	//Check if a string is very basic A-Z a-z 0-9
	static bool validateBasicString(string toCheck);

	//Check if a string is an integer
	static bool stringIsNumber(string toCheck);

	//Check if a string is a float
	static bool stringIsFloat(string toCheck);

	//Convert primitives to a string
	template <class T>
	static string toString(T value) {
		stringstream ss;
		ss << value;
		return ss.str();
	}

	//Create a random something
	template <class T>
	static T random(T min, T max) {
		return ((float)rand()/(float)(RAND_MAX))*(max-min)+min;
	}

	//Move string to memory with max length specified
	static void MoveString(string original, char * copyLocation, int maxSize);

	//A POD type of vertex
	PACK(
	struct PODVec4 {
		float x;
		float y;
		float z;
		float w;
	});
	PACK(
	struct PODVec3 {
		float x;
		float y;
		float z;
	});
	PACK(
	struct PODVec2 {
		float x;
		float y;
	});
};

//Convert a normal vector to a POD
Utilities::PODVec4 toPOD(vec4 vector);
Utilities::PODVec3 toPOD(vec3 vector);
Utilities::PODVec2 toPOD(vec2 vector);



