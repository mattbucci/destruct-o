// Returns the path to a directory where Destructo should stick save files

#include <Foundation/Foundation.h>
#include "stdafx.h"
#include "OS.h"

#if !(defined __IPHONEOS__)
#error da fuck how you doin dis
#endif

// Return a save file directory
std::string OS::SaveDirectory()
{
    // Look for the documents directory
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    
    // Get the documents directory
    NSString *documentsDirectory = [paths firstObject];
    
    // Convert the Objective-C string into a c++ string
    std::string saveDir = [documentsDirectory cStringUsingEncoding:NSASCIIStringEncoding];
    
    // Return the magical save directory
    return saveDir + "/";
}
