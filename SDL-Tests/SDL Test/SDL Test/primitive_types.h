//
//  primitive_types.h
//  SDL Test
//
//  Created by Nathaniel Lewis on 1/28/14.
//  Copyright (c) 2014 E1FTW Games. All rights reserved.
//

#ifndef SDL_Test_primitive_types_h
#define SDL_Test_primitive_types_h

#include <SDL2/SDL_opengl.h>

// 3 dimensional vector type for structs
typedef struct __vector2
{
    // components
    float x;
    float y;
} Vector2;

// 3 dimensional vector type for structs
typedef struct __vector3
{
    // components
    float x;
    float y;
    float z;
} Vector3;

// 3 dimensional vector type for structs
typedef struct __vector4
{
    // components
    float x;
    float y;
    float z;
    float w;
} Vector4;

#endif
