
//This header is only included under iOS
//so lets assume opengl es 2, and use all of apple's extensions

#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glBindVertexArray glDeleteVertexArraysOES
#define glGenVertexArrays glGenVertexArraysOES

#define glVertexAttribDivisor glVertexAttribDivisorEXT
#define glDrawElementsInstanced glDrawElementsInstancedEXT

