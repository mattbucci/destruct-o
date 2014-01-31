
#include "stdafx.h"
#include "OS.h"

long long OS::performanceFrequency = 0;

//These are universal
void OS::WaitUntil(double until) {
	while (OS::Now() < until) {
		//Waste time...
	}
}




#ifdef  _WIN32
#include <Windows.h>

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

#ifdef __APPLE__
#include <unistd.h>
#endif

//UNIX stuff
#include <sys/time.h>
double OS::Now(void) {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);
	return (double)tv.tv_sec+(double)tv.tv_usec/(1000.0*1000.0);
}

void OS::SleepTime(double seconds) {
	usleep((int)(seconds*1000.0*1000.0));
}

#endif
