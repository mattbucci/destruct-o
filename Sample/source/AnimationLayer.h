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

#ifndef __ANIMATION_LAYER_H__
#define __ANIMATION_LAYER_H__

#include "stdafx.h"
#include "AnimationSource.h"
#include "AnimationState.h"

// Forward declaration of the animation controller object
class AnimationController;

/** 
 * AnimationLayer is a finite state machine dedicated to controlling the animation 
 * of a model's skeleton.  It holds a collection of AnimationState objects which 
 * do the actual work of animating.
 */
class AnimationLayer : public AnimationSource
{
public:
    /**
     * Comparison operator to sort layers for priority
     */
    struct PriorityComparator
    {
        bool operator()(AnimationLayer *a, AnimationLayer *b)
        {
            return (a->Priority() > b->Priority());
        }
    };
    
    /** State storage types */
    typedef std::map<std::string, AnimationState *> state_store;
    
protected:
    /**
     * AnimationLayer depends on components provides by the animation controller
     * and requires a reference to it
     */
    AnimationController& controller;
    
    /** The name of the animation layer */
    std::string name;
    
    /** The priority of this layer */
    int priority;
    
    /** The states that exist in this layer */
    state_store states;
    
public:
    /**
     * Empty constructor.  Builds an empty animation layer bound to an animation
     * controller.
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(AnimationController& _controller);
    
    /**
     * Copy constructor.  Duplicates the animation layer
     * @param layer The animation layer to duplicate
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(const AnimationLayer& layer, AnimationController& _controller);
    
    /**
     * Deserialization constructor.  Builds an animation layer bound to an animation
     * controller from a serialized Json blob
     * @param value The Json object to deserialize from
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(const Json::Value& value, AnimationController& _controller);
    
    /**
     * Standard deconstructor.  Releases heap memory
     */
    ~AnimationLayer();
    
    /**
     * Update the state of the model layer
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);
    
    /**
     * Get the name of this animation layer
     * @return Const reference to the name of the layer
     */
    const std::string& Id() const;
    
    /**
     * Get the priority of this animation layer
     * @return Const reference to the name of the layer
     */
    const int& Priority() const;
    
    /**
     * Accessor for the animation controller of this layer 
     * @return Reference to the animation controller of the layer
     */
    const AnimationController& Controller() const;
};

#endif
