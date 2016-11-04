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
#include "ModelInstance.h"
#include "OS.h"

//#define __MODELINSTANCE_PRINT_LOGS__

/**
 * Create a model instance for a model
 * @param _model Model to provide an instance of
 */
ModelInstance::ModelInstance(Model *_model)
    : model(_model), transform(Transform()), controller(new AnimationController()), animation(NULL)
{
    // Bind the animation controller
    controller->Bind(_model->Skeleton());

    // Bind the animator
    animation.Bind(_model->Skeleton());
}

/**
 * Copy constructor
 * @param instance ModelInstance to duplicate (does not duplicate model)
 */
ModelInstance::ModelInstance(const ModelInstance& instance)
    : model(instance.model), transform(instance.transform), controller(instance.controller), animation(instance.animation)
{

}

/**
 * Update the state of this model instance
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void ModelInstance::Update(double delta, double now)
{
    // Make sure our model is uploaded
    model->Update(delta, now);

    // Update the animation controller
    controller->Update(delta, now);

    // Update the animation test
    animation.Update(delta, now);
}

/*
* Modify the animation directly, cause we be crazy man
*/
AnimationClip & ModelInstance::Animation()
{
	return animation;
}

/**
 * Draw this model instance with a particular shader
 * @param program the shader to draw this model with
 */
void ModelInstance::Draw(MaterialProgram *program)
{
    // Push the model transform onto the program's matrix stack
    program->Model.PushMatrix();
    program->Model.SetMatrix(transform.TransformMatrix());
    //program->Model.Apply();

    // DUCT TAPE SOLUTON WARNING
    if(controller->Layers().size())
    {
        // Draw the model (If we have animation layers
        model->Draw(program, *(controller->Skeleton()));
    }

    // otherwise fallback on the older system
    else
    {
        animation.Skeleton()->Recalculate();
        model->Draw(program, *(animation.Skeleton()));
    }

    // Remove the translation
    program->Model.PopMatrix();
}

/**
 * Get a reference to the transform of the instance
 * @return reference to transform
 */
Transform& ModelInstance::GetTransform()
{
    return transform;
}

/**
 * Get a const pointer to the model object of the instance
 * @return const pointer to model
 */
const Model* ModelInstance::GetModel() const
{
    return model.get();
}

/**
 * Get a reference to the animation controller of this model instance
 * @return Reference to the animation controller of this model instance
 */
AnimationController* ModelInstance::Controller()
{
    return controller;
}

/**
 * Static method to construct a model instance from a manifest file
 * @param value The Json serialized value to build this model instance from
 * @return Pointer to a model instance encapsulating the entry
 */
ModelInstance* ModelInstance::LoadManifestEntry(const Json::Value& model, TextureCache& textureCache)
{
    // Get the model information
    std::string name = (model)["name"].asString();
    std::string path = (model)["path"].asString();
    std::string directory = (model)["directory"].asString();
    bool compressed = (model)["compressed"].asBool();

    // The offset transform
    Transform transform;

    // Search for an offset transform
    const Json::Value& offset = (model)["offset"];

    // If an offset was specified in the manifest
    if(offset != Json::Value::null && offset.isObject())
    {
        transform = Transform(offset);
    }

    // Load the model from json or compressed json
    ModelInstance *instance = NULL;
    if(compressed)
    {
        instance = new ModelInstance(Model::LoadFromCompressedJsonFile(directory, path, transform, textureCache));
    } else
    {
        instance = new ModelInstance(Model::LoadFromJsonFile(directory, path, transform, textureCache));
    }

    // Search for an animation controller
    const Json::Value& controller = (model)["controller"];

    // If an animation controller was defined, load it
    if(controller != Json::Value::null && controller.isObject())
    {
        // Forcibly load the controller
        delete instance->controller;
        instance->controller = new AnimationController(controller, instance->model.get());
    }

#if (defined __MODELINSTANCE_PRINT_LOGS__)
    // Print out the animations
    for(Model::animation_const_iterator it = instance->model->Animations().begin(); it != instance->model->Animations().end(); it++)
    {
        cout << "Animation: " << it->first << " is " << it->second->Length() << " seconds" << endl;
    }
#endif

    // Return the created instance
    return instance;
}



// TEST TEST TEST
bool ModelInstance::PlayAnimation(const std::string name)
{
    // Get an iterator to the animation we want to play
    Model::animation_const_iterator anim = model->Animations().find(name);

    // If the iterator is invalid, this failed, so escape
    if(anim == model->Animations().end())
    {
        return false;
    }

    // Play this animation
    animation.SetAnimation(anim->second);

    // Play the animation
    animation.Play(true, OS::Now());

    // Return success
    return true;
}

const Node* ModelInstance::Skeleton() const
{
    return animation.Skeleton();
}
