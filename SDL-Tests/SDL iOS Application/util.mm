//
//  util.m
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 E1FTW Games. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "util.hpp"

std::string defaultResourceDirectory()
{
    // Bundle lookup
#ifdef __MACOSX__
    std::string defaultResourceDirectory = [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/Contents/Resources"] cStringUsingEncoding:NSASCIIStringEncoding];
#elif __IPHONEOS__
    std::string defaultResourceDirectory = [[[NSBundle mainBundle] bundlePath] cStringUsingEncoding:NSASCIIStringEncoding];
#endif
    return defaultResourceDirectory;
}