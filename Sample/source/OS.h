
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
	static long long performanceFrequency;
};