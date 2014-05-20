
#include "stdafx.h"
#include "OS.h"

//These are universal
void OS::WaitUntil(double until) {
	while (OS::Now() < until) {
		//Waste time...
	}
}


//Constructs the given path out of folders
//Not OS dependant (put that part in construct directory)
void OS::BuildPath(string path) {
	string curPath;
	for (auto c : path) {
		if (c == '/')
			constructDirectory(curPath);
		curPath += c;
	}
}

#ifdef  _WIN32
bool dirExists(const std::string& dirName_in) {
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	return (ftyp != INVALID_FILE_ATTRIBUTES) && (ftyp & FILE_ATTRIBUTE_DIRECTORY);
}


//This is OS dependant!
void OS::constructDirectory(const std::string& path) {
	//Check if it exists first
	if (!dirExists(path))
		CreateDirectory(path.c_str(),NULL);
}

#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// Create a directory
void OS::constructDirectory(const std::string& path)
{
    // Check if the directory already exists
    struct stat s;
    if(stat(path.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
    {
        // It is a directory, get out
        return;
    }
    
    // Make the directory (-rwxrwx---)
    mkdir(path.c_str(), S_IRWXU | S_IRWXG);
}

#endif


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
