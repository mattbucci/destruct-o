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
    : AnimationSource() , controller(_controller), name("null"), priority(0), states(state_store()), activeState(NULL), transitionState(NULL), transitionStartTime(0.0), transitionLength(0.0)
{
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller.InitialSkeleton());
}

/**
 * Copy constructor.  Duplicates the animation layer
 * @param layer The animation layer to duplicate
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(const AnimationLayer& layer, AnimationController& _controller)
    : AnimationSource(), controller(_controller), name(layer.name), priority(layer.priority), states(state_store()), activeState(NULL), transitionState(NULL), transitionStartTime(layer.transitionStartTime), transitionLength(layer.transitionLength)
{
    // Duplicate other important stuff (like states)
    for(state_store::const_iterator it = layer.states.begin(); it != layer.states.end(); it++)
    {
        // Allocate a state
        AnimationState *state = new AnimationState(*(it->second), *this);
        
        // Store the state
        states[state->Id()] = state;
    }
    
    // Lookup the current transition animations
    if(layer.transitionState)
    {
        transitionState = states[layer.transitionState->Id()];
    }
    if(layer.activeState)
    {
        activeState = states[layer.activeState->Id()];
    }
    
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
    : AnimationSource(), controller(_controller), states(state_store()), activeState(NULL), transitionState(NULL), transitionStartTime(0.0), transitionLength(0.0)
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
    
    // Get the states
    const Json::Value& serializedStates = value["states"];
    
    // If we have states, iterate through them and get their contents
    if(!serializedStates.isNull() && serializedStates.isArray())
    {
        // Iterate through them
        for(Json::Value::const_iterator it = serializedStates.begin(); it != serializedStates.end(); it++)
        {
            // Allocate the state
            AnimationState *state = new AnimationState(*it, *this);
            
            // Store this state
            states[state->Id()] = state;
        }
    }
    
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller.InitialSkeleton());
}

/**
 * Standard deconstructor - releases all heap memory
 */
AnimationLayer::~AnimationLayer()
{
    // Delete all the layers
    for(state_store::iterator it = states.begin(); it != states.end(); it++)
    {
        delete it->second;
    }
}

/**
 * Binds the animation layer to a skeleton
 * @param root the root node of the transform tree to bind to
 */
void AnimationLayer::Bind(const Node* root)
{
    // Call the base bind function (we're extending it)
    AnimationSource::Bind(root);
    
    // Update all the states with the new skeleton
    
}

/**
 * Update the state of the model layer
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationLayer::Update(double delta, double now)
{
    // Update all the layers
    for(state_store::iterator it = states.begin(); it != states.end(); it++)
    {
        it->second->Update(delta, now);
    }
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