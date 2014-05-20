
#pragma once

#include "stdafx.h"

#include "Utilities.h"

class GLModel;
class GLEffectProgram;

class GLEffectVertexGroup {
public:
	PACK(
	struct EffectVertex {
		Utilities::PODVec3 vertex;
		Utilities::PODVec2 textureCoordinate;
		Utilities::PODVec4 color;
	});
private:
	//The number of vertices in the vertex group
	int vertexCount;

	//The glenum representing how the vertices are drawn (ex: GL_TRIANGLES)
	GLenum gltype;

	GLuint vertexArray;
	GLuint vertexBuffer;

	EffectVertex * vertices;
	//If the size changed this is true
	bool sizeChanged;
public:
	GLEffectVertexGroup(GLenum gltype, int vertexCount);

	~GLEffectVertexGroup();

	//Resize the vertex group
	void ResizeVertexGroup(int newSize);
	
	//For retrieving vertices
	EffectVertex & vat(int index);

	//Draw this object
	//set reupload to true if you want to force reupload
	void Draw(GLEffectProgram * shader, bool reupload);
};