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

#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "stdafx.h"
#include "Transform.h"
#include "Node.h"
#include "Model.h"
#include "AnimationSource.h"
#include "AnimationLayer.h"

#include <json/json.h>

// AnimationController handles the animation control subsystem.
class AnimationController : public AnimationSource
{
public:
    // The parameter value data structure (input type to the animation controller)
    struct Parameter
    {
        // The data storage for the parameter
        typedef union
        {
            bool  boolean;
            float number;
        } Value;

        // The types of values
        typedef enum : unsigned char
        {
            kTypeFloat = 0,
            kTypeBool = 1,
        } Type;

        // The name of the parameter
        std::string name;

        // The type of the parameter
        Type type;

        // The value of the parameter
        Value value;

        // Generic constructor
        Parameter();

        // Copy construtor
        Parameter(const Parameter& parameter);

        // Deserialization constructor
        Parameter(const Json::Value& value);
    };

    // Parameter storage typess
    typedef std::map<std::string, Parameter> parameter_store;
    typedef parameter_store::iterator        parameter_iterator;
    typedef parameter_store::const_iterator  parameter_const_iterator;

    // Layer storage types
    typedef std::map<std::string, AnimationLayer *> layer_store;
    typedef std::vector<AnimationLayer *>           layer_vector;
    typedef layer_store::iterator                   layer_iterator;
    typedef layer_store::const_iterator             layer_const_iterator;
    typedef std::priority_queue<AnimationLayer *, layer_vector, AnimationLayer::PriorityComparator> layer_queue;

private:
    /**
     * The parameter storage.  The state of the system is affected by them
     */
    parameter_store parameters;

    /**
     * The layer storage.  The state of the system is affected by them
     */
    layer_store layers;

    /**
     * A sorted list of layers by priority
     */
    layer_queue layerQueue;

    /**
     * The model this animation controller is controlling
     */
    const Model* model;

public:
    /**
     * Do nothing constructor - just to handle meshes which don't have controllers
     */
    AnimationController();

    /**
     * Create an animation controller as a copy of another animation controller
     * @param value Json value to deserialize this animation controller from
     * @param skeleton Root node of the skeleton of the transform tree to animation
     */
    AnimationController(const AnimationController& controller);

    /**
     * Create an animation controller from a serialized Json source (yeah, no manual creation
     * yet).
     * @param value Json value to deserialize this animation controller from
     * @param model Model to bind to this animation controller
     */
    AnimationController(const Json::Value& value, const Model *model);

    /**
     * Destroy an animation controller and any heap allocated memory associated
     */
    virtual ~AnimationController();

    /**
     * Assignment operator.  Animation controller assumes the state of the other
     * @param controller Controller to set this controller to
     */
    AnimationController& operator= (const AnimationController& controller);

    /**
     * Binds the animation controller to a skeleton
     * @param root the root node of the transform tree to bind as a skeleton
     */
    void Bind(const Node *root) override;

    /**
     * Update the animation controller.  Performs animation calculations and updates the skeleton
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now) override;

    /**
     * Get the model this animation controller is associated with
     * @return Const pointer to the model this controller will animate
     */
    const Model* GetModel() const;

    /**
     * Get a reference to the layers of this animation controller
     * @return Reference to the layer storage
     */
    layer_store& Layers();

    // parameter functions
    parameter_const_iterator GetParameter(const std::string& key) const;
    const bool GetBoolean(const std::string& key, bool fallback = false) const;
    const float GetFloat(const std::string& key, float fallback = 0.0f) const;
    void SetBoolean(const std::string& key, bool value);
    void SetFloat(const std::string& key, float value);
};

#endif
