#include "stdafx.h"
#ifdef __ANDROID__
#include "AndroidGLEW.h"
#include <EGL/egl.h>



//These are standard extensions available on many android platforms
MYBIND_GLBINDVERTEXARRAY glBindVertexArray;
MYBIND_GLGENVERTEXARRAY glGenVertexArrays;
MYBIND_GLDELETEVERTEXARRAY glDeleteVertexArrays;

//These are undocumented extensions available on only a few platforms (or platforms which support GLES3)
MYBIND_GLVERTEXATTRIBDIVISOR glVertexAttribDivisor;
MYBIND_GLDRAWELEMENTSINSTANCED glDrawElementsInstanced;

//Attempt to retrieve the linking of every requested GL function
#define SET(glFunctionName,functionType) if (!(glFunctionName = (functionType)eglGetProcAddress(#glFunctionName))) {return false;}

//sets up vertex arrays
//fails if they aren't available
static bool setupAndroidOpenGLBasics() {
	SET(glBindVertexArray,MYBIND_GLBINDVERTEXARRAY);
	SET(glGenVertexArrays,MYBIND_GLGENVERTEXARRAY);
	SET(glDeleteVertexArrays,MYBIND_GLDELETEVERTEXARRAY);

	return true;
}

//Sets up instanced rendering
//fails if its not available
static bool setupAndroidOpenGLAdvanced() {
	SET(glVertexAttribDivisor,MYBIND_GLVERTEXATTRIBDIVISOR);
	SET(glDrawElementsInstanced,MYBIND_GLDRAWELEMENTSINSTANCED);

	return true;
}

//sets up

int initAndroidGlew() {
	//Android requires VAO, set those up now
	if (!setupAndroidOpenGLBasics())
		return -1;

	//Android now supports instanced drawing
	//set it up now if supported
	if (setupAndroidOpenGLAdvanced())
		return 31;
	else 
		return 20;
}
#endif