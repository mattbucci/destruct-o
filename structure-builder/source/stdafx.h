// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//Basic I/O
#include <iostream>

//Handy containers
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>

//Other utilities
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>

using namespace std;

//Define assert for non windows based platforms
#ifndef _ASSERTE
#define _ASSERTE assert
#endif

//Too lazy to allow Unicode support (only effects windows)
#ifdef UNICODE
#undef UNICODE
#endif

#ifndef __MOBILE__
#include <GL/glew.h>
#endif

//GLEW now handles all of opengl
#ifndef WIN32
//for OSX
#ifdef __APPLE__
//Prevent SDL from using gl.h
#define __gl_h_
#endif
//For Android
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
#endif
#endif


#include <SDL2/SDL.h>


//GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//Basic types used everywhere
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

//Useful defines
#define ADDSUFFIX_2(x,y) x ## y
#define ADDSUFFIX_1(x,y) ADDSUFFIX_2(x,y)

//These defines are designed to allow the user to make warnings
//in visual studio. They can be ignored
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "

//A variable which at runtime can be used to figure out what version is running
//3 is 33
//2 is 20
extern int OpenglVersion;
extern int GLSLVersion;

// The active joystick
extern SDL_Joystick *joystick;

// Clamp function
template <typename T>
inline float clamp(T x, T a, T b)
{
    return x < a ? a : (x > b ? b : x);
}

#undef main
