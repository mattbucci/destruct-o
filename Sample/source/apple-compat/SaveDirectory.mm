// Returns the path to a directory where Destructo should stick save files

#include <Foundation/Foundation.h>
#include "../OS.h"

// If we are running on iOS
#if (defined __IPHONEOS__)

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

// If we are compiling for Mac OS X
#else

// Pull in the application support directory handler
#include "NSFileManager+ApplicationSupport.h"

// Return a save file directory
std::string OS::SaveDirectory()
{
    // Get the application support directory
    NSString *documentsDirectory = [[NSFileManager defaultManager] applicationSupportDirectory];
    
    // Convert the Objective-C string into a c++ string
    std::string saveDir = [documentsDirectory cStringUsingEncoding:NSASCIIStringEncoding];
    
    // Return the magical save directory
    return saveDir + "/";
}

#endif
