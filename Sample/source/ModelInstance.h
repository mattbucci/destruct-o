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
#include "AnimationController.h"

#include "AnimationClip.h"

#include <memory>

class ModelInstance
{
    /** Model we are providing an instance of */
    std::shared_ptr<Model>    model;
    
    /** The transform to render the model at */
    Transform                 transform;
    
    /** The animation controller of this instance */
    AnimationController       controller;
    
    /** SUPER DUPER TESTING MODE SHIT */
    AnimationClip             animation;
public:
    /**
     * Create a model instance for a model
     * @param _model Model to provide an instance of
     */
    ModelInstance(Model *_model);
    
    /** 
     * Copy constructor
     * @param instance ModelInstance to duplicate (does not duplicate model)
     */
    ModelInstance(const ModelInstance& instance);
    
    /**
     * Update the state of this model instance
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);
    
	/*
	* Modify the animation directly, cause we be crazy man
	*/
	AnimationClip & Animation();

    /**
     * Draw this model instance with a particular shader
     * @param program the shader to draw this model with
     */
    void Draw(MaterialProgram *program);
    
    /**
     * Get a reference to the transform of the model
     * @return reference to transform
     */
    Transform& GetTransform();
    
    /**
     * Get a pointer to the model this instance uses
     * @return pointer to model object
     */
    const Model* GetModel() const;
    
    /**
     * Get a reference to the animation controller of this model instance
     * @return Reference to the animation controller of this model instance
     */
    AnimationController& Controller();
    
    /**
     * Static method to construct a model instance from a manifest file
     * @param value The Json serialized value to build this model instance from
     * @return Pointer to a model instance encapsulating the entry
     */
    static ModelInstance* LoadManifestEntry(const Json::Value& model, TextureCache& textureCache);
    
    /** Test stuff **/
    bool PlayAnimation(const std::string name);
    const Node* Skeleton() const;
};

#endif
