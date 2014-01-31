
#pragma once

#include "stdafx.h"

class GLDynamicVertexGroup {
	//The number of vertices in the vertex group
	int vertexCount;

	//The glenum representing how the vertices are drawn (ex: GL_TRIANGLES)
	GLenum gltype;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint textureBuffer;

	vec3 * vertices;
	vec3 * vertexNormals;
	vec2 * textureCoords;
protected:

	void UpdateGroup(GLenum gltype, int vertexCount);
	
	//For modifying vertices
	vec3 & vat(const int index);
	//For modifying normals
	vec3 & nat(const int index);
	//For modifying texture coordinates
	vec2 & xat(const int index);

	//If the size changed since the last draw call
	bool sizeChanged;
public:
	GLDynamicVertexGroup();
	~GLDynamicVertexGroup();


	//void Bind(mat4 transformation);
	void Draw();
};