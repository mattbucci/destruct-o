//
//  util.m
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 E1FTW Games. All rights reserved.
//

#include "util.hpp"

#import <Foundation/Foundation.h>

std::string defaultResourceDirectory()
{
    // Bundle lookup
    std::string defaultResourceDirectory = [[[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/Contents/Resources"] cStringUsingEncoding:NSASCIIStringEncoding];
    return defaultResourceDirectory;
}