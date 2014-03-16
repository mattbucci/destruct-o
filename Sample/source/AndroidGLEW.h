
#pragma once

#include "stdafx.h"

#ifdef __ANDROID__

typedef void (*MYBIND_GLBINDVERTEXARRAY)(GLuint array);
typedef void (*MYBIND_GLGENVERTEXARRAY)(GLsizei n, const GLuint *arrays);
typedef void (*MYBIND_GLDELETEVERTEXARRAY)(GLsizei n, GLuint *arrays);
typedef void (*MYBIND_GLVERTEXATTRIBDIVISOR)(GLuint index, GLuint divisor);
typedef void (*MYBIND_GLDRAWELEMENTSINSTANCED)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount);

//These are standard extensions available on many android platforms
extern MYBIND_GLBINDVERTEXARRAY glBindVertexArray;
extern MYBIND_GLGENVERTEXARRAY glGenVertexArrays;
extern MYBIND_GLDELETEVERTEXARRAY glDeleteVertexArrays;

//These are undocumented extensions available on only a few platforms (or platforms which support GLES3)
extern MYBIND_GLVERTEXATTRIBDIVISOR glVertexAttribDivisor;
extern MYBIND_GLDRAWELEMENTSINSTANCED glDrawElementsInstanced;

//call only after a gl context has been returned
//only one call is necessary
//returns true if successful, otherwise false
int initAndroidGlew();

#endif