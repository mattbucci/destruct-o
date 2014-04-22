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

#ifndef __ANIMATION_STATE_H__
#define __ANIMATION_STATE_H__

#include "stdafx.h"
#include "Transform.h"
#include "Node.h"
#include "AnimationSource.h"

#include <json/json.h>

// Forward declaration of the animation layer object
class AnimationLayer;

/**
 * The animation state object represents a singular state an animation layer
 * can currently exist in.  It represents a
 */
class AnimationState
{
    /**
     * AnimationState depends on functionality from the animation layers.  Namely,
     * this is the ability to load animation clips, transition to other states,
     * and interpret parameters.  We therefore need a reference to our parent layer
     */
    AnimationLayer& layer;
    
    /** The name of the animation layer */
    std::string name;
    
public:
    /**
     * Standard constructor.  Create an empty anmation state
     * @param layer The animation layer this state will reside on
     */
    AnimationState(AnimationLayer& layer);
    
    /**
     * Copy constructor.  Create an animation state as a duplicate and reparent
     * to the provided layer
     * @param state The animation state to duplicate
     * @param layer The animation layer this state will reside on
     */
    AnimationState(const AnimationState& state, AnimationLayer& layer);
    
    /**
     * Deserialization constructor.  Create an animation state from the serialized
     * json data
     * @param value The serialized animation state to load
     * @param layer The animation layer this state will reside on
     */
    AnimationState(const Json::Value& value, AnimationLayer& layer);
    
    /**
     * Standard deconstructor.  Cleans up any created animation clips
     * that have been bound to the state
     */
    ~AnimationState();
    
    /**
     * Update the state of the model layer
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);
    
    /**
     * Get the name of this animation state
     * @return Const reference to the name of the state
     */
    const std::string& Id() const;
};

#endif
