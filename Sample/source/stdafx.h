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
#include <stdint.h>
#include <functional>

using namespace std;

//For android or any other platform which requires it
//this converts "cout" into an actually loggable format
#include "AuxLogger.h"

//Define assert for non windows based platforms
#ifndef _ASSERTE
#define _ASSERTE assert
#endif

//Too lazy to allow Unicode support (only effects windows)
#ifdef UNICODE
#undef UNICODE
#endif

//Opengl3 for nonwindows platforms
#ifndef WIN32
//for OSX
#ifdef __APPLE__
#include <OpenGL/gl3.h>
//Prevent SDL from using gl.h
#define __gl_h_
#endif
//For Android
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#endif
#endif


#include <SDL/SDL.h>


//Define mobile platforms
//iOS would also go here
#ifdef __ANDROID__
#define __MOBILE__
#endif

//GLEW (should be replaced)
#ifdef WIN32
#include <GL/glew.h>
#endif

//SDL 2
#ifndef __ANDROID__
#include <SDL/SDL_opengl.h>
#else
#include <SDL/SDL_opengles2.h>
#endif

//GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//Basic types used everywhere
using glm::vec2;
using glm::vec3;
using glm::vec4;
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

//Handle the SDL entry point
//For mobile applications the SDL entry point is actually used
#ifndef __MOBILE__
//SDL defines main for some reason. This is undesirable
#undef main
#else
#define main SDL_main
#endif
