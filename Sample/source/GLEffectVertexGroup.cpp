#include "stdafx.h"
#include "GLEffectVertexGroup.h"
#include "GLModel.h"
#include "GLEffectProgram.h"

//Build the arrays to create the vertex group
GLEffectVertexGroup::GLEffectVertexGroup(GLenum gltype, int vertexCount) {
	vertices = new EffectVertex[vertexCount];
	

	this->gltype = gltype;
	this->vertexCount = vertexCount;

	//If there are no vertices, abort
	if (vertexCount == 0)
		return;

	sizeChanged = true;

	//Generate the opengl buffers representing this vertex group
	//will also need a buffer for textures in the future
	glGenBuffers(1,&vertexBuffer);
	glGenVertexArrays(1,&vertexArray);

	//Buffer junk data (doesn't matter if data is uninitialized)
	//This allows the allocation to happen once
	glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(EffectVertex), vertices, GL_DYNAMIC_DRAW );
}

void GLEffectVertexGroup::ResizeVertexGroup(int newSize) {
	delete[] vertices;

	this->vertexCount = newSize;

	vertices = new EffectVertex[vertexCount];
}

GLEffectVertexGroup::~GLEffectVertexGroup() {
	glDeleteBuffers(1,&vertexBuffer);
	glDeleteVertexArrays(1,&vertexArray);

	delete [] vertices;
}
	
//For retrieving vertices
GLEffectVertexGroup::EffectVertex & GLEffectVertexGroup::vat(int index) {
	_ASSERTE((index >= 0) && (index < vertexCount));
	return vertices[index];

}
void GLEffectVertexGroup::Draw(GLEffectProgram * shader, bool reupload) {
	_ASSERTE(vertexBuffer >= 0);
	_ASSERTE(vertexArray >= 0);

	//If there are no vertices, abort
	if (vertexCount == 0)
		return;

	//Rebind the array to bring them into the current context
	glBindVertexArray ( vertexArray );

	if (sizeChanged) {
		sizeChanged = false;
		//vertices
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferData ( GL_ARRAY_BUFFER, vertexCount*sizeof(EffectVertex), vertices, GL_STATIC_DRAW );
		//vertex positions
		glEnableVertexAttribArray ( shader->AttributeVertex() );
		glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(EffectVertex), (void*)offsetof(EffectVertex,vertex)  );
		// texture coordinates
		glEnableVertexAttribArray ( shader->AttributeTexture() );
		glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, sizeof(EffectVertex), (void*)offsetof(EffectVertex,textureCoordinate)  );
		// colors
		glEnableVertexAttribArray ( shader->AttributeColor() );
		glVertexAttribPointer ( shader->AttributeColor(), 4, GL_FLOAT, GL_FALSE,sizeof(EffectVertex), (void*)offsetof(EffectVertex,color)  );
	}
	else if (reupload) {
		//vertices
		glBindBuffer ( GL_ARRAY_BUFFER, vertexBuffer );
		glBufferSubData ( GL_ARRAY_BUFFER, 0, vertexCount*sizeof(EffectVertex), vertices );
		//vertex positions
		glEnableVertexAttribArray ( shader->AttributeVertex() );
		glVertexAttribPointer ( shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, sizeof(EffectVertex), (void*)offsetof(EffectVertex,vertex)  );
		// texture coordinates
		glEnableVertexAttribArray ( shader->AttributeTexture() );
		glVertexAttribPointer ( shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, sizeof(EffectVertex), (void*)offsetof(EffectVertex,textureCoordinate)  );
		// colors
		glEnableVertexAttribArray ( shader->AttributeColor() );
		glVertexAttribPointer ( shader->AttributeColor(), 4, GL_FLOAT, GL_FALSE,sizeof(EffectVertex), (void*)offsetof(EffectVertex,color)  );
	}

	
	glDrawArrays( gltype, 0, vertexCount );
	glBindVertexArray(0);
}
