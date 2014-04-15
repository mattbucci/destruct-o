
#pragma once

#include "stdafx.h"

class GLModel;
class GL2DProgram;

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

	//whether or not buffers need to be reuploaded
	bool dirty;
public:
	GL2DVertexGroup(GLenum gltype, int vertexCount);

	~GL2DVertexGroup();

	//Resize the vertex group
	void ResizeVertexGroup(int newSize);
	
	//For retrieving vertices
	vec2 svat(const int index);
	vec2 sxat(const int index);

	//For modifying vertices
	void svat(const int index, vec2 vertex);
	//For modifying textures
	void sxat(const int index, vec2 textureCoordinate);

	//void Bind(mat4 transformation);
	void Draw(GL2DProgram * shader);
};