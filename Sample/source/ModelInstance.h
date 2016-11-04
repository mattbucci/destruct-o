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
    AnimationController      *controller;

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
    AnimationController* Controller();

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
