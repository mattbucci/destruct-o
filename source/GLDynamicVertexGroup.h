
#pragma once

#include "stdafx.h"

class GL3DProgram;

class GLDynamicVertexGroup {
	//The number of vertices in the vertex group
	int vertexCount;

	//The glenum representing how the vertices are drawn (ex: GL_TRIANGLES)
	GLenum gltype;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint textureBuffer;
	GLuint positionBuffer;

	vec3 * vertices;
	vec3 * vertexNormals;
	vec2 * textureCoords;

	bool sizeChanged;
protected:

	void UpdateGroup(GLenum gltype, int vertexCount);
	
	//For modifying vertices
	vec3 & vat(const int index);
	//For modifying normals
	vec3 & nat(const int index);
	//For modifying texture coordinates
	//note due to the nature of the 3d shader
	//texture coordinates must go from 0 to 4 (NOT from 0 to 1)
	vec2 & xat(const int index);
public:
	GLDynamicVertexGroup();
	~GLDynamicVertexGroup();


	void Draw(GL3DProgram * shader);
};