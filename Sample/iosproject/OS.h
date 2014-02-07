
#pragma once
#include "stdafx.h"

//Interfaces for functions which differ in implementation per OS
class OS {
public:
	//Sleeps the given number of seconds
	static void SleepTime(double seconds);
	//Wastes cpu time until the specified time
	static void WaitUntil(double until);
	//Returns the current time in seconds
	static double Now(void);
private:
#ifdef  _WIN32
	//For windows 
	//the number of ticks in a second
	static long long performanceFrequency;
#else
	//For unix (OSX/Android/linux)
	//the starting time in seconds of the app
	static long long startTimeSeconds;
#endif
};