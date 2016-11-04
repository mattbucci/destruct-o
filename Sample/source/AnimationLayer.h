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
    AnimationController *controller;

    /** The name of the animation layer */
    std::string name;

    /** The priority of this layer */
    int priority;

    /** The states that exist in this layer */
    state_store states;

    /** The current active state */
    AnimationState *activeState;

    /** The state being transitioned from */
    AnimationState *transitionState;

    /** The transition state "on deck" (will transition, not blending) */
    AnimationState *transitionStateOnDeck;

    /** The time at which the transition began */
    double          transitionStartTime;

    /** The length of the transition */
    double          transitionLength;

public:
    /**
     * Empty constructor.  Builds an empty animation layer bound to an animation
     * controller.
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(AnimationController *_controller);

    /**
     * Copy constructor.  Duplicates the animation layer
     * @param layer The animation layer to duplicate
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(const AnimationLayer& layer, AnimationController *_controller);

    /**
     * Deserialization constructor.  Builds an animation layer bound to an animation
     * controller from a serialized Json blob
     * @param value The Json object to deserialize from
     * @param _controller The animation controller to bind this layer to
     */
    AnimationLayer(const Json::Value& value, AnimationController *_controller);

    /**
     * Standard deconstructor.  Releases heap memory
     */
    ~AnimationLayer();

    /**
     * Binds the animation layer to a skeleton
     * @param root the root node of the transform tree to bind to
     */
    void Bind(const Node* root) override;

    /**
     * Update the state of the model layer
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);

    /**
     * Cause the animation controller to transition to a state
     * @param state the state to transition to
     * @param now the current simulated time
     */
    void Transition(const std::string& state, double now);

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
    const AnimationController* Controller() const;
};

#endif
