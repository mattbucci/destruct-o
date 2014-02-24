
#pragma once

//Used for logging if cout is not available
//I don't know about iOS but traditional cout doesn't work on android
#ifdef __ANDROID__

#define endl "\n"

#include <android/log.h>
#include <sstream>

class AuxLogging {
	stringstream streamBuffer;
public:
	AuxLogging(){}

	template <typename T>
	AuxLogging &operator<<(const T &a) {
		stringstream str;
		str << a;
		string c = str.str();
		//Look through str for a "\n" if you find one, flush the streamBuffer
		for (int i = 0; i < (int)c.size();) {
			if (c[i] == '\n') {
				streamBuffer << c.substr(0,i);
				__android_log_write(ANDROID_LOG_INFO, "NDK", streamBuffer.str().c_str());
				//printf("%s\n",streamBuffer.str().c_str());
				streamBuffer.str("");
				i++;
				c = c.substr(i,c.size()-i);
				i = 0;
			}
			else
				i++;
		}
		streamBuffer << c;
		return *this;
	}
};

extern AuxLogging AuxLogger;

//Replace cout
#define cout AuxLogger

#endif