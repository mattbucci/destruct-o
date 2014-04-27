// Returns the path to a directory where Destructo should stick save files

#include "stdafx.h"
#include "OS.h"

// Only include this file on non apple platforms
#if !(defined __IPHONEOS__) && !(defined __APPLE__)

#if !(defined __ANDROID__)

// Return a save file directory
std::string OS::SaveDirectory()
{
    // Find the magical save directoy
    std::string saveDir = "./";
    
    // Return the magical save directory
    return saveDir;
}

#else

// Return a save file directory
std::string OS::SaveDirectory()
{
    // Find the magical save directoy
    std::string saveDir = "/mnt/sdcard/";
    
    // Return the magical save directory
    return saveDir;
}

#endif

#endif