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
#include "AnimationLayer.h"
#include "AnimationController.h"

/**
 * Empty constructor.  Builds an empty animation layer bound to an animation
 * controller.
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(AnimationController& _controller)
    : AnimationSource() , controller(_controller), name("null"), priority(0)
{
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller.InitialSkeleton());
}

/**
 * Copy constructor.  Duplicates the animation layer
 * @param layer The animation layer to duplicate
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(AnimationLayer& layer, AnimationController& _controller)
    : AnimationSource(), controller(_controller), name(layer.name), priority(layer.priority)
{
    // Duplicate other important stuff (like states)
    
    
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller.InitialSkeleton());
}

/**
 * Deserialization constructor.  Builds an animation layer bound to an animation
 * controller from a serialized Json blob
 * @param value The Json object to deserialize from
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(const Json::Value& value, AnimationController& _controller)
    : AnimationSource(), controller(_controller)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("AnimationLayer::AnimationLayer(const Json::Value& value, AnimationController& _controller) - value must be a object");
    }
    
    // Get the name of the layer
    name = value["name"].asString();
    
    // Get the priority of the layer
    priority = value["priority"].asInt();
    
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller.InitialSkeleton());
}

/**
 * Update the state of the model layer
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationLayer::Update(double delta, double now)
{
    
}

/**
 * Get the name of this animation layer
 * @return Const reference to the name of the layer
 */
const std::string& AnimationLayer::Id() const
{
    return name;
}

/**
 * Get the priority of this animation layer
 * @return Const reference to the name of the layer
 */
const int& AnimationLayer::Priority() const
{
    return priority;
}

/**
 * Accessor for the animation controller of this layer
 * @return Reference to the animation controller of the layer
 */
const AnimationController& AnimationLayer::Controller() const
{
    return controller;
}