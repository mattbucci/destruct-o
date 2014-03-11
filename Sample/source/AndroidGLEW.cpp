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

bool initAndroidGlew() {
	SET(glBindVertexArray,MYBIND_GLBINDVERTEXARRAY);
	SET(glGenVertexArrays,MYBIND_GLGENVERTEXARRAY);
	SET(glDeleteVertexArrays,MYBIND_GLDELETEVERTEXARRAY);


	SET(glVertexAttribDivisor,MYBIND_GLVERTEXATTRIBDIVISOR);
	SET(glDrawElementsInstanced,MYBIND_GLDRAWELEMENTSINSTANCED);
	//If you make it here the load was successful
	return true;
}
#endif