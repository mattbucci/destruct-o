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
#include <array>
#include <queue>

//Other utilities
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>

//for threading
#include <mutex>
#include <thread>

using namespace std;

//Disable the rest of includes if in the unit testing system
#ifndef __UNIT_TESTING__

//Define mobile platforms
//iOS would also go here
#if (defined __ANDROID__) || (defined __IPHONEOS__)
#define __MOBILE__
#endif

//Define assert for non windows based platforms
#ifndef _ASSERTE
#define _ASSERTE assert
#endif

//visual studio is retarded
#ifdef WIN32
#define noexcept throw()
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
// GLEW defines __gl_h_ which will trip OS X into thinking the old gl header was included
#undef __gl_h_
#endif
//For Android
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include "AndroidGLEW.h"
#endif
#endif


#include <SDL2/SDL.h>

//SDL 2
#ifndef __MOBILE__
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles2.h>
//Apple's custom extensions
#ifdef __IPHONEOS__
#include "IOSGL.h"
#endif
#endif

//For android or any other platform which requires it
//this converts "cout" into an actually loggable format
//Must be included after SDL
#include "AuxLogger.h"


//GLM
#include <glm/glm.hpp>
//#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

//Basic types used everywhere
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::quat;

//The save system
//since it is used in many many places
#include "Savable.h"
#include "ReflectionStore.h"

//Simulation delta
#define SIMULATION_DELTA .01f

//Useful defines
#define ADDSUFFIX_2(x,y) x ## y
#define ADDSUFFIX_1(x,y) ADDSUFFIX_2(x,y)

//These defines are designed to allow the user to make warnings
//in visual studio. They can be ignored
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ " ( " __STR1__(__LINE__) " ) : Warning Msg: "

//Allow packing of structures in GCC and in VisualC++ not sure about clang
#ifdef WIN32
//Assume visual studio
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else
//Assume GCC (I have no idea how to make this work with clang)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

//Handle the SDL entry point
//For mobile applications the SDL entry point is actually used
#ifndef __MOBILE__
//SDL defines main for some reason. This is undesirable
#undef main
#else
#define main SDL_main
#endif

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

#endif
