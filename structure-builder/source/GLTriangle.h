
#pragma once

#include "stdafx.h"

template <class T>
class GLTriangle {
public:
	GLTriangle() {

	}
	GLTriangle(T a, T b, T c) {
		A = a;
		B = b;
		C = c;
	}
	GLTriangle(const GLTriangle & toCopy) {
		A = toCopy.A;
		B = toCopy.B;
		C = toCopy.C;
	}

	GLTriangle & operator=(const GLTriangle & toCopy) {
		A = toCopy.A;
		B = toCopy.B;
		C = toCopy.C;
		return *this;
	}


	T A;
	T B;
	T C;
};

typedef GLTriangle<vec2> GLTriangle2;
typedef GLTriangle<vec3> GLTriangle3;