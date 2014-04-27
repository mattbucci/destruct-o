//
//  NSFileManager+ApplicationSupport.h
//  VoxEngine
//
//  Created by Nathaniel Lewis on 4/27/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include <Foundation/Foundation.h>

@interface NSFileManager (ApplicationSupport)

- (NSString *)findOrCreateDirectory:(NSSearchPathDirectory)searchPathDirectory inDomain:(NSSearchPathDomainMask)domainMask appendPathComponent:(NSString *)appendComponent error:(NSError **)errorOut;

// Get the application support directory on Mac OS X
- (NSString *)applicationSupportDirectory;

@end
