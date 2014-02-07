//
//  cube.h
//  SDL Test
//
//  Created by Nathaniel Lewis on 1/28/14.
//  Copyright (c) 2014 E1FTW Games. All rights reserved.
//

#ifndef SDL_Test_cube_h
#define SDL_Test_cube_h

#include "primitive_types.h"

// The verticies of the cube
const Vector3 cube_verticies[] =
{
    // Face 1
    {-0.5f,  0.5f, -0.5f},  // E
    { 0.5f,  0.5f, -0.5f},  // F
    {-0.5f,  0.5f,  0.5f},  // A
    { 0.5f,  0.5f,  0.5f},  // B
    
    // Face 2
    {-0.5f,  0.5f,  0.5f},  // A
    { 0.5f,  0.5f,  0.5f},  // B
    {-0.5f, -0.5f,  0.5f},  // C
    { 0.5f, -0.5f,  0.5f},  // D
    
    // Face 3
    { 0.5f,  0.5f,  0.5f},  // B
    { 0.5f,  0.5f, -0.5f},  // F
    { 0.5f, -0.5f,  0.5f},  // D
    { 0.5f, -0.5f, -0.5f},  // H
    
    // Face 4
    { 0.5f,  0.5f, -0.5f},  // F
    {-0.5f,  0.5f, -0.5f},  // E
    { 0.5f, -0.5f, -0.5f},  // H
    {-0.5f, -0.5f, -0.5f},  // G
    
    // Face 5
    {-0.5f,  0.5f, -0.5f},  // E
    {-0.5f,  0.5f,  0.5f},  // A
    {-0.5f, -0.5f, -0.5f},  // G
    {-0.5f, -0.5f,  0.5f},  // C
    
    // Face 6
    {-0.5f, -0.5f,  0.5f},  // C
    { 0.5f, -0.5f,  0.5f},  // D
    {-0.5f, -0.5f, -0.5f},  // G
    { 0.5f, -0.5f, -0.5f},  // H
};

// The normals of the cube
const Vector3 cube_normals[] =
{
    // Face 1
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    
    // Face 2
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    
    // Face 3
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    
    // Face 4
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    
    // Face 5
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    
    // Face 6
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
};

// The texture coordinates of the cube
const Vector2 cube_texcoords[] =
{
    // Face 1
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    
    // Face 2
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    
    // Face 3
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    
    // Face 4
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    
    // Face 5
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    
    // Face 6
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f},
};

// The element array for the coordinates of the cube
const GLubyte cube_elements[] =
{
    // Face 1
    0, 1, 3,
    3, 2, 0,
    
    // Face 2
    4, 5, 7,
    7, 6, 4,
    
    // Face 3
    8, 9, 11,
    11, 10, 8,
    
    // Face 4
    12, 13, 15,
    15, 14, 12,
    
    // Face 5
    16, 17, 19,
    19, 18, 16,
    
    // Face 6,
    20, 21, 23,
    23, 22, 20,
};

#endif
