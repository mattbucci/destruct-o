#include "stdafx.h"
#include "Utilities.h"

//Check if a string is very basic A-Z a-z 0-9
bool Utilities::validateBasicString(string toCheck) {
	if (toCheck.length() <= 1)
		return false;

	//Check if the name contains any invalid characters
	for (unsigned int i = 0; i < toCheck.length(); i++) {
		char c = toCheck[i];
		if (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z'))  || ((c >= 'A') && (c <= 'Z')) || (c == '.') || (c == '_'))
			continue;

		return false;
	}

	return true;
}

//Check if a string is a number
bool Utilities::stringIsNumber(string toCheck) {
	//Check if the name contains any invalid characters
	for (unsigned int i = 0; i < toCheck.length(); i++) {
		char c = toCheck[i];
		if ((c >= '0') && (c <= '9'))
			continue;

		return false;
	}

	return true;
}

bool Utilities::stringIsFloat(string toCheck) {
	//Check if the name contains any invalid characters
	for (unsigned int i = 0; i < toCheck.length(); i++) {
		char c = toCheck[i];
		if (((c >= '0') && (c <= '9')) || (c == '.') || (c == '-'))
			continue;

		return false;
	}

	return true;
}

void Utilities::MoveString(string original, char * copyLocation, int maxSize) {
	memset(copyLocation,0,maxSize);
	memcpy(copyLocation,original.c_str(),min(31,maxSize));
}