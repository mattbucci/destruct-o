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
public:
    /**
     * Utility class to manager the transition data
     */
    struct Transition
    {
        // Possible comparison functions
        typedef enum : unsigned char
        {
            // The exit fun
            kComparisonFunctionExit = 0,
            kComparisonFunctionEquals = 1,
            kComparisonFunctionLess = 2,
            kComparisonFunctionGreater = 3,
        } ComparisonFunction;

        // The target state
        std::string target;

        // The input parameter
        std::string parameter;

        // Comparison function
        ComparisonFunction function;

        // The value
        union
        {
            bool  boolean;
            float number;
        } value;

        // Create the transition object
        Transition();

        // Deserialize
        Transition(const Json::Value& value);
    };

    // Storage type for the transitions
    typedef std::vector<Transition> transition_vector;

protected:
    /**
     * AnimationState depends on functionality from the animation layers.  Namely,
     * this is the ability to load animation clips, transition to other states,
     * and interpret parameters.  We therefore need a reference to our parent layer
     */
    AnimationLayer *layer;

    /** The name of the animation layer */
    std::string name;

    /** The animation source that this state encapsulates */
    AnimationSource *source;

    /** The transitions this state uses */
    transition_vector transitions;

    /** is this a one shot or repetitive action */
    bool loop;

public:
    /**
     * Standard constructor.  Create an empty anmation state
     * @param layer The animation layer this state will reside on
     */
    AnimationState(AnimationLayer *layer);

    /**
     * Copy constructor.  Create an animation state as a duplicate and reparent
     * to the provided layer
     * @param state The animation state to duplicate
     * @param layer The animation layer this state will reside on
     */
    AnimationState(const AnimationState& state, AnimationLayer *layer);

    /**
     * Deserialization constructor.  Create an animation state from the serialized
     * json data
     * @param value The serialized animation state to load
     * @param layer The animation layer this state will reside on
     */
    AnimationState(const Json::Value& value, AnimationLayer *layer);

    /**
     * Standard deconstructor.  Cleans up any created animation clips
     * that have been bound to the state
     */
    ~AnimationState();

    /**
     * Binds the animation state to a skeleton (the state itself is not bound,
     * but rather its children)
     * @param root the root node of the transform tree to bind to
     */
    void Bind(const Node* root);

    /**
     * Update the state of the model layer
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now, bool comparison = true);

    /**
     * Event to signal that this state is being considered for transition
     * @param now the current simulated time
     */
    void WillTransition(double now);

    /**
     * Event to signal that this state has become active
     * @param now the current simulated time
     */
    void DidTransition(double now);

    /**
     * Get the name of this animation state
     * @return Const reference to the name of the state
     */
    const std::string& Id() const;

    /**
     * Get a const pointer to the local skeleton
     * @return const pointer to the local skeleton
     */
    const Node* Skeleton() const;

    /**
     * Get a const reference to the local skeleton bone lookup map
     * @return const reference to the local skeleton bone lookup map
     */
    const Node::flattreemap& Bones() const;
};

#endif
