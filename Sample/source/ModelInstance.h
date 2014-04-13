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

#ifndef __MODEL_INSTANCE_H__
#define __MODEL_INSTANCE_H__

#include "stdafx.h"
#include "Model.h"

class ModelInstance
{
    /** Model we are providing an instance of */
    Model     *model;
    
    /** A clone of the skeleton of the model */
    Node      *skeleton;
    
    /** The transform to render the model at */
    Transform  transform;
    
    /** The currently playing animation */
    const Animation *animation;
    
    /** The time at which the animation was started */
    float animationStartTime;
public:
    /**
     * Create a model instance for a model
     * @param _model Model to provide an instance of
     */
    ModelInstance(Model *_model);
    
    /**
     * Update the state of this model instance
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);
    
    /**
     * Draw this model instance with a particular shader
     * @param program the shader to draw this model with
     */
    void Draw(GLMeshProgram *program);
    
    /**
     * Play an animation in this model by name
     * @param name the name of the animation to play
     * @return false if the animation did not exist, true if the animation is now playing
     */
    bool PlayAnimation(const std::string name);
    
    /**
     * Get a reference to the transform of the model
     * @return reference to transform
     */
    Transform& GetTransform();
};

#endif
