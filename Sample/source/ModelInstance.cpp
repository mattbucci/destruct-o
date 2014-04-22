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

#include "stdafx.h"
#include "ModelInstance.h"
#include "OS.h"

/**
 * Create a model instance for a model
 * @param _model Model to provide an instance of
 */
ModelInstance::ModelInstance(Model *_model)
    : model(_model), transform(Transform()), controller(AnimationController()), animation(NULL)
{
    // Bind the animation controller
    controller.Bind(_model->Skeleton());
    
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
    controller.Update(delta, now);
    
    // Update the animation test
    animation.Update(delta, now);
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
    program->Model.Apply();
    
    // Draw the model
    //model->Draw(program, *(controller.Skeleton()));
    model->Draw(program, *(animation.Skeleton()));
    
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
AnimationController& ModelInstance::Controller()
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
        instance->controller = AnimationController(controller);
        instance->controller.Bind(instance->model->Skeleton());
    }
    
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
