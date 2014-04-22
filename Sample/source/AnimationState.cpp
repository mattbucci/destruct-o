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

/**
 * Standard constructor.  Create an empty anmation state
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(AnimationLayer& _layer)
: layer(_layer), name("")
{
    
}

/**
 * Copy constructor.  Create an animation state as a duplicate and reparent
 * to the provided layer
 * @param state The animation state to duplicate
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(const AnimationState& state, AnimationLayer& _layer)
    : layer(_layer), name(state.name)
{
    
}

/**
 * Deserialization constructor.  Create an animation state from the serialized
 * json data
 * @param value The serialized animation state to load
 * @param layer The animation layer this state will reside on
 */
AnimationState::AnimationState(const Json::Value& value, AnimationLayer& _layer)
    : layer(_layer), name("")
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("AnimationState::AnimationState(const Json::Value& value, AnimationLayer& _layer) - value must be a object");
    }
    
    // Get the name of the layer
    name = value["name"].asString();
}

/**
 * Standard deconstructor.  Cleans up any created animation clips
 * that have been bound to the state
 */
AnimationState::~AnimationState()
{
    
}

/**
 * Update the state of the animation state
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationState::Update(double delta, double now)
{
    
}

/**
 * Get the name of this animation state
 * @return Const reference to the name of the state
 */
const std::string& AnimationState::Id() const
{
    return name;
}
