/*
 *  Copyright 2014 Nathaniel Lewis
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "ModelInstance.h"

/**
 * Create a model instance for a model
 * @param _model Model to provide an instance of
 */
ModelInstance::ModelInstance(Model *_model)
    : model(_model), skeleton(new Node(*(_model->Skeleton()))), transform(Transform())
{
    
}

/**
 * Update the state of this model instance
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void ModelInstance::Update(double delta, double now)
{
    model->Update(delta, now);
}

/**
 * Draw this model instance with a particular shader
 * @param program the shader to draw this model with
 */
void ModelInstance::Draw(GLMeshProgram *program)
{
    // Push the model transform onto the program's matrix stack
    program->Model.PushMatrix();
    program->Model.SetMatrix(transform.TransformMatrix());
    program->Model.Apply();
    
    // Draw the model
    model->Draw(program, *skeleton);
    
    // Remove the translation
    program->Model.PopMatrix();
}

/**
 * Get a reference to the transform of the model
 * @return reference to transform
 */
Transform& ModelInstance::GetTransform()
{
    return transform;
}