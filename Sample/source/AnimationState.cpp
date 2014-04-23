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
#include "AnimationState.h"
#include "AnimationLayer.h"
#include "AnimationController.h"

#include "AnimationClip.h"
#include "AnimationBlendGroup.h"

/**
 * Standard constructor.  Create an empty anmation state
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(AnimationLayer& _layer)
    : layer(_layer), name(""), source(new AnimationSource())
{
    
}

/**
 * Copy constructor.  Create an animation state as a duplicate and reparent
 * to the provided layer
 * @param state The animation state to duplicate
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(const AnimationState& state, AnimationLayer& _layer)
    : layer(_layer), name(state.name), source(NULL)
{
    // Duplicate the transition information
    
    
    // Duplicate the animation source
    AnimationClip *animation = dynamic_cast<AnimationClip *>(state.source);
    
    // If we are an animation clip
    if(animation)
    {
        source = new AnimationClip(*animation);
        return;
    }
    
    // unsupported animation source
    throw new std::runtime_error("AnimationState::AnimationState(const AnimationState& state, AnimationLayer& _layer) - can not duplicate unsupported animation source");
}

/**
 * Deserialization constructor.  Create an animation state from the serialized
 * json data
 * @param value The serialized animation state to load
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(const Json::Value& value, AnimationLayer& _layer)
    : layer(_layer), name(""), source(new AnimationSource())
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("AnimationState::AnimationState(const Json::Value& value, AnimationLayer& _layer) - value must be a object");
    }
    
    // Get the name of the layer
    name = value["name"].asString();
    
    // Load the transitions
    
    // Load the animation source
    const Json::Value& animationSource = value["source"];
    
    // If we have a serialized animation source
    if(animationSource.isObject())
    {
        // Get the type
        const std::string type = animationSource["type"].asString();
        
        // If this is an animation clip, load it
        if(type == "AnimationClip")
        {
            // Get the name of the animation
            const std::string animationName = animationSource["value"].asString();
            
            // Get an iterator to the animation
            Model::animation_const_iterator animationIt = layer.Controller().GetModel()->Animations().find(animationName);
            
            // If we found the animation
            if(animationIt != layer.Controller().GetModel()->Animations().end())
            {
                // Create a new AnimationClip based on the input animation
                source = new AnimationClip(animationIt->second);
            }
            
            // Otherwise throw an exception
            else
            {
                throw std::runtime_error("AnimationState::AnimationState(const Json::Value& value, AnimationLayer& _layer) - unrecognized animation clip requested");
            }
        }
        
        // If this is a blend group, load it
        else if(type == "AnimationBlendGroup")
        {
            
        }
    }
}

/**
 * Standard deconstructor.  Cleans up any created animation clips
 * that have been bound to the state
 */
AnimationState::~AnimationState()
{
    delete source;
}

/**
 * Binds the animation state to a skeleton (the state itself is not bound,
 * but rather its children)
 * @param root the root node of the transform tree to bind to
 */
void AnimationState::Bind(const Node* root)
{
    // Bind the skeleton to the source
    source->Bind(root);
}

/**
 * Update the state of the animation state
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationState::Update(double delta, double now)
{
    // Update the target source object
    source->Update(delta, now);
    
    // Figure out if we should transition
    
}

/**
 * Event to signal that the animation is transitioning to this state
 * @param now the current simulated time
 */
void AnimationState::WillTransition(double now)
{
    // Is the source an animation clip
    AnimationClip *animation = dynamic_cast<AnimationClip *>(source);
    
    // If we are an animation clip
    if(animation)
    {
        // Cause the animation to begin playing (also resets the pose)
        animation->Play(true, now);
        
        // Get out
        return;
    }
    
    // If we don't recognize the source type, just reset the source to initial pose
    source->Reset();
}

/**
 * Get the name of this animation state
 * @return Const reference to the name of the state
 */
const std::string& AnimationState::Id() const
{
    return name;
}

/**
 * Get a const pointer to the local skeleton
 * @return const pointer to the local skeleton
 */
const Node* AnimationState::Skeleton() const
{
    return source->Skeleton();
}

/**
 * Get a const reference to the local skeleton bone lookup map
 * @return const reference to the local skeleton bone lookup map
 */
const Node::flattreemap& AnimationState::Bones() const
{
    return source->Bones();
}
