
#pragma once

#include "stdafx.h"

class GLModel;

class GLVertexGroup {
	//The number of vertices in the vertex group
	int vertexCount;

	//The glenum representing how the vertices are drawn (ex: GL_TRIANGLES)
	GLenum gltype;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint normalBuffer;

	vec3 * vertices;
	vec3 * vertexNormals;
public:
	GLVertexGroup(GLenum gltype, int vertexCount);
	~GLVertexGroup();
	
	//For modifying vertices
	vec3 & vat(const int index);
	//For modifying normals
	vec3 & nat(const int index);

	//void Bind(mat4 transformation);
	void Draw();
};