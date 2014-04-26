// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "lodepng.h"

using namespace std;

#define NINE_PATCH_COMPILER

class vec2 {
public:
	float x, y;
	vec2() {
		x = y= 0;
	}
	vec2(float x, float y) {
		this->x = x;
		this->y = y;
	}
};


// TODO: reference additional headers your program requires here
