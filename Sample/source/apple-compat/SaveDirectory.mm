/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
