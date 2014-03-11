
#pragma once

#include "stdafx.h"

#ifdef __ANDROID__

typedef void (*MYBIND_GLBINDVERTEXARRAY)(GLuint array);
typedef void (*MYBIND_GLGENVERTEXARRAY)(GLsizei n, const GLuint *arrays);
typedef void (*MYBIND_GLDELETEVERTEXARRAY)(GLsizei n, GLuint *arrays);

extern MYBIND_GLBINDVERTEXARRAY glBindVertexArray;
extern MYBIND_GLGENVERTEXARRAY glGenVertexArrays;
extern MYBIND_GLDELETEVERTEXARRAY glDeleteVertexArrays;

//call only after a gl context has been returned
//only one call is necessary
//returns true if successful, otherwise false
bool initAndroidGlew();

#endif