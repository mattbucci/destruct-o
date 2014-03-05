
#include "stdafx.h"
#include "OS.h"

//These are universal
void OS::WaitUntil(double until) {
	while (OS::Now() < until) {
		//Waste time...
	}
}




#ifdef  _WIN32
#include <Windows.h>
long long OS::performanceFrequency = 0;

double OS::Now(void) {
	if (performanceFrequency == 0) {
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		performanceFrequency = li.QuadPart;
	}
	LARGE_INTEGER ti;
	QueryPerformanceCounter(&ti);
	return (double)ti.QuadPart/(double)performanceFrequency;
}

void OS::SleepTime(double seconds) {
	Sleep((DWORD)(seconds*1000.0));
}

#else

#if (defined __APPLE__) || (defined __ANDROID__) || (defined __LINUX__)
#include <unistd.h>
#endif

//UNIX stuff
#include <sys/time.h>

long long OS::startTimeSeconds = -1;

double OS::Now(void) {
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv,&tz);

	if (startTimeSeconds < 0)
		startTimeSeconds = tv.tv_sec;

	return (double)(tv.tv_sec-startTimeSeconds)+(double)tv.tv_usec/(1000.0*1000.0);
}

void OS::SleepTime(double seconds) {
	usleep((int)(seconds*1000.0*1000.0));
}

#endif
