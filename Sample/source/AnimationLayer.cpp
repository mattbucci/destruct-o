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
#include "AnimationLayer.h"
#include "AnimationController.h"

#define DEFAULT_TRANSITION_DURATION 0.25
//#define __ANIMLAYER_PRINT_LOGS__

/**
 * Empty constructor.  Builds an empty animation layer bound to an animation
 * controller.
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(AnimationController *_controller)
    : AnimationSource() , controller(_controller), name("null"), priority(0), states(state_store()), activeState(NULL), transitionState(NULL), transitionStateOnDeck(NULL), transitionStartTime(0.0), transitionLength(0.0)
{
    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller->InitialSkeleton());
}

/**
 * Copy constructor.  Duplicates the animation layer
 * @param layer The animation layer to duplicate
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(const AnimationLayer& layer, AnimationController *_controller)
    : AnimationSource(), controller(_controller), name(layer.name), priority(layer.priority), states(state_store()), activeState(NULL), transitionState(NULL), transitionStateOnDeck(NULL), transitionStartTime(layer.transitionStartTime), transitionLength(layer.transitionLength)
{
    // Duplicate other important stuff (like states)
    for(state_store::const_iterator it = layer.states.begin(); it != layer.states.end(); it++)
    {
        // Allocate a state
        AnimationState *state = new AnimationState(*(it->second), this);

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
    if(layer.transitionStateOnDeck)
    {
        transitionStateOnDeck = states[layer.transitionStateOnDeck->Id()];
    }

    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller->InitialSkeleton());
}

/**
 * Deserialization constructor.  Builds an animation layer bound to an animation
 * controller from a serialized Json blob
 * @param value The Json object to deserialize from
 * @param _controller The animation controller to bind this layer to
 */
AnimationLayer::AnimationLayer(const Json::Value& value, AnimationController *_controller)
    : AnimationSource(), controller(_controller), states(state_store()), activeState(NULL), transitionState(NULL), transitionStateOnDeck(NULL), transitionStartTime(0.0), transitionLength(0.0)
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
            AnimationState *state = new AnimationState(*it, this);

            // Store this state
            states[state->Id()] = state;
        }
    }

    // Bind this source to the initial skeleton of the animation controller
    Bind(_controller->InitialSkeleton());

    // Transition to the default state of the layer
    Transition(value["default"].asString(), 0);
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

    // Bind all the states with the new skeleton
    for(state_store::iterator it = states.begin(); it != states.end(); it++)
    {
        it->second->Bind(root);
    }
}

/**
 * Update the state of the model layer
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationLayer::Update(double delta, double now)
{
    // If we don't have an active state, there is nothing to do
    if(!activeState)
    {
        return;
    }

    // Update the active state if we are not in a case 4 state transition
    activeState->Update(delta, now, (transitionStateOnDeck == NULL));

    // If we are in a case 4 animation transition, update the state on deck
    if(transitionStateOnDeck != NULL)
    {
        // Update the state on deck
        transitionStateOnDeck->Update(delta, now);
    }

    // If we have a state transition occuring
    if(transitionState)
    {
        // Calculate the current progress of the transitional phase
        double progress = (now - transitionStartTime) / transitionLength;

        // If we have completed the current transitional phase
        if(progress >= 1.0)
        {
            // If we are currently in a case 4 transition (there is a state on deck)
            if(transitionStateOnDeck != NULL)
            {
                // The transitional state is now replaced by the active state
                transitionState = activeState;

                // The active state is now the on deck transition state
                activeState = transitionStateOnDeck;

                // Disable the on deck state (clear case 4)
                transitionStateOnDeck = NULL;

                // The desired transition length is set
                transitionLength = DEFAULT_TRANSITION_DURATION;

                // The transition start time is stored
                transitionStartTime = now;

                // Alert the active state that it has become active
                activeState->DidTransition(now);

                // Reset the progress to zero
                progress = 0.0;

#if (defined __ANIMLAYER_PRINT_LOGS__)
                // Log
                cout << "Transition Case 4 ==> ";
                cout << "OnDeck Now Active: " << activeState->Id() << " ";
                cout << "Transition: " << transitionState->Id() << endl;
#endif
            }

            // If it was a normal transition
            else
            {
                // Kill the transition state, we are now in the active mode
                transitionState = NULL;

#if (defined __ANIMLAYER_PRINT_LOGS__)
                cout << "Transition Complete" << endl;
#endif
                // Copy the active state's skeleton into the local skeleton
                for(Node::flattreemap::iterator it = skeletonTable.begin(); it != skeletonTable.end(); it++)
                {
                    // Get the local transform of the cooresponding bone in the active layer
                    it->second->LocalTransform() = activeState->Bones().find(it->first)->second->LocalTransform();
                }

                // Return
                return;
            }
        }

        // Update the transition state's content
        transitionState->Update(delta, now, false);

        // Set the skeleton as a blend between the two
        for(Node::flattreemap::iterator it = skeletonTable.begin(); it != skeletonTable.end(); it++)
        {
            // Get the local transform of the cooresponding bone in the transition layer
            const Transform& a = transitionState->Bones().find(it->first)->second->LocalTransform();

            // Get the local transform of the cooresponding bone in the active layer
            const Transform& b = activeState->Bones().find(it->first)->second->LocalTransform();

            // Store the interpolated transform
            it->second->LocalTransform() = Transform::Interpolate(a, b, (float)progress);
        }

        // Return from the update method
        return;
    }

    else
    {
        // Copy the active state's skeleton into the local skeleton
        for(Node::flattreemap::iterator it = skeletonTable.begin(); it != skeletonTable.end(); it++)
        {
            // Get the local transform of the cooresponding bone in the active layer
            it->second->LocalTransform() = activeState->Bones().find(it->first)->second->LocalTransform();
        }
    }
}

/**
 * Cause the animation controller to transition to a state
 * @param state the state to transition to
 * @param now the current simulated time
 */
void AnimationLayer::Transition(const std::string& state, double now)
{
    // Perform a lookup of the state
    state_store::iterator stateIt = states.find(state);

    // If the iterator is invalid, complain
    if(stateIt == states.end())
    {
        // Throw an exception
        throw std::runtime_error("void AnimationLayer::Transition(const std::string& state) - reference state does not exist");
    }

    /**
     * The animation transitions are a little complicated to keep smooth.  Here is the solution =P
     * There are up to three states that will be considered.  The "Active State" is the currently
     * running state.  The "Transition State" is the old active state being phased out.  The "Transition
     * on deck" is the state poised to become active.  In order to make things smooth, we allow the
     * current transition to finish before moving to something else.  The logic is as follows
     *
     * If there is no active state, the requested state immediately becomes active.  this should only ever happen once (case 1)
     *
     * If there is an active state and no transition is in progress, the active state is moved to the transition state,
     * and requested state is set as active.  The timers are setup to handle this (case 2)
     *
     * If there is an active state AND a transition in progress, one of two things can happen.
     *     If the target state is the currently phasing out state, the transition will be reversed, and the remaining
     *     animation time will be reversed (i.e. if the transition was 20% complete, the reversal would be 80% complete) (case 3)
     *
     *     If the target state is a new state, it will be set "on deck" and the active state will loose transitional
     *     priviledges (which the on deck transition will gain eventually) (case 4)
     */

    // Special initial condition
    if(activeState == NULL)
    {
        // Store the new active state
        activeState = stateIt->second;

        // Cause the active state to enter the transitional phase
        activeState->WillTransition(now);
        activeState->DidTransition(now);

        // Log
#if (defined __ANIMLAYER_PRINT_LOGS__)
        cout << "Transition Case 1 ==> ";
        cout << "Active: " << activeState->Id() << endl;
#endif
    }

    // If there is in fact an active state, perform the considerations
    else
    {
        // If there is no current transition, setup the state transition
        if(transitionState == NULL)
        {
            // Currently active state is moved off to the inactive, transitional phase
            transitionState = activeState;

            // The transition start time is stored
            transitionStartTime = now;

            // The desired transition length is set
            transitionLength = DEFAULT_TRANSITION_DURATION;

            // The new active state is the desired state
            activeState = stateIt->second;

            // Cause the active state to enter the transitional phase
            activeState->WillTransition(now);
            activeState->DidTransition(now);

            // Log
#if (defined __ANIMLAYER_PRINT_LOGS__)
            cout << "Transition Case 2 ==> ";
            cout << "Active: " << activeState->Id() << " ";
            cout << "Transition: " << transitionState->Id() << endl;
#endif
        }

        // If there is a transition in progress
        else
        {
            // Is the desired state the state currently being phased out?
            if(transitionState == stateIt->second)
            {
                // Reverse the current transition time (moving back to old state)
                double transitionTime = 1.0 - ((now - transitionStartTime) / transitionLength);

                // Calculate a fudged transition start time
                transitionStartTime = now - (transitionTime * transitionLength);

                // Store the new transition state
                transitionState = activeState;

                // Active state is now the old state
                activeState = stateIt->second;

                // Clear the on deck state
                transitionStateOnDeck = NULL;

                // Log
#if (defined __ANIMLAYER_PRINT_LOGS__)
                cout << "Transition Case 3 ==> ";
                cout << "Active: " << activeState->Id() << " ";
                cout << "Transition: " << transitionState->Id() << endl;
#endif
            }

            // Otherwise we shall place this new state "on deck"
            else
            {
                // Place the transition on deck
                transitionStateOnDeck = stateIt->second;

                // Prepare it for animation
                transitionStateOnDeck->WillTransition(now);

                // Log
#if (defined __ANIMLAYER_PRINT_LOGS__)
                cout << "Transition Case 4 ==> ";
                cout << "Active: " << activeState->Id() << " ";
                cout << "Transition: " << transitionState->Id() << " ";
                cout << "On Deck: " << transitionStateOnDeck->Id() << endl;
#endif
            }
        }

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
const AnimationController* AnimationLayer::Controller() const
{
    return controller;
}
