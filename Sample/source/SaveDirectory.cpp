// Returns the path to a directory where Destructo should stick save files

#include "stdafx.h"
#include "OS.h"

#if (defined __IPHONEOS__)
#error you son of a bitch dont include this file on ios you gonna break this shitty shit
#endif

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
