
#pragma once

#include "stdafx.h"

class GLModel;

class GL2DVertexGroup {
	//The number of vertices in the vertex group
	int vertexCount;

	//The glenum representing how the vertices are drawn (ex: GL_TRIANGLES)
	GLenum gltype;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint textureBuffer;

	vec2 * vertices;
	vec2 * vertexTextureCoords;
public:
	GL2DVertexGroup(GLenum gltype, int vertexCount);

	~GL2DVertexGroup();

	//Resize the vertex group
	void ResizeVertexGroup(int newSize);
	
	//For modifying vertices
	vec2 & vat(const int index);
	//For modifying textures
	vec2 & xat(const int index);

	//void Bind(mat4 transformation);
	void Draw();
};