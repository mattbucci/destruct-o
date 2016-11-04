/**
 * Copyright (c) 2016, Nathaniel R. Lewis, Anthony Popelar, Matt Bucci, Brian Bamsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdafx.h"
#include "GLTerrainProgram.h"

GLTerrainProgram::GLTerrainProgram(GLCommonShaderFile * commonShader, string vertexShaderPath, string fragmentShaderPath) :
	GLProgram(commonShader, vertexShaderPath,fragmentShaderPath),
	colors(glGetUniformLocation(programId,"materials")),
	Model(glGetUniformLocation(programId, "M")),
	Camera(glGetUniformLocation(programId, "P"), glGetUniformLocation(programId, "V"), glGetUniformLocation(programId, "VP")),
	Fog(programId),
	Acid(glGetUniformLocation(programId,"uCurrentTime"),glGetUniformLocation(programId,"uAcidFactor"))
{
	attributeIndexTexture = glGetAttribLocation(programId,"vTex");
	attributeIndexVertex = glGetAttribLocation(programId,"vVert");
	attributeIndexMaterial = glGetAttribLocation(programId,"vMaterial");
    uniformMatrixModelView = glGetUniformLocation(programId, "MV");
    uniformMatrixModelViewProjection = glGetUniformLocation(programId, "MVP");
};

void GLTerrainProgram::UseProgram(void) {
	GLProgram::UseProgram();
	colors.Apply();
}

const GLint GLTerrainProgram::AttributeTexture() {
	return attributeIndexTexture;
}
const GLint GLTerrainProgram::AttributeVertex() {
	return attributeIndexVertex;
}
const GLint GLTerrainProgram::AttributeMaterial() {
	return attributeIndexMaterial;
}

void GLTerrainProgram::Apply()
{
    // Work in progress matrix
    mat4 storage = Model.GetMatrix();

    // If we have a model view matrix
    if(uniformMatrixModelView >= 0)
    {
        storage = Camera.ViewMatrix() * storage;
        glUniformMatrix4fv(uniformMatrixModelView, 1, GL_FALSE, &storage[0][0]);
    }

    // If we have a model view projection matrix
    if(uniformMatrixModelViewProjection >= 0)
    {
        // If the matrix was already premultiplied
        if(uniformMatrixModelView >= 0)
        {
            storage = Camera.ProjectionMatrix() * storage;
        } else
        {
            // Do the full chain
            storage = Camera.ProjectionMatrix() * Camera.ViewMatrix() * storage;
        }

        // Set the mvp matrix
        glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, GL_FALSE, &storage[0][0]);
    }
}
