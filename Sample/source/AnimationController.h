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

#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "stdafx.h"
#include "AnimationLayer.h"
#include "Transform.h"
#include "Node.h"

#include <json/json.h>

// AnimationController handles the animation control subsystem.
class AnimationController
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
    
    // The type used for storing the parameters
    typedef std::map<std::string, Parameter> parameter_store;
    typedef parameter_store::iterator        parameter_iterator;
    typedef parameter_store::const_iterator  parameter_const_iterator;
    
private:
    /**
     * The parameter storage.  The state of the system is affected by them
     */
    parameter_store parameters;
    
public:
    /**
     * Create an animation controller as a copy of another
     * @param value Json value to deserialize this animation controller from
     * @param skeleton Root node of the skeleton of the transform tree to animation
     */
    AnimationController(const AnimationController& controller);
    
    /**
     * Create an animation controller from a serialized Json source (yeah, no manual creation
     * yet).  
     * @param value Json value to deserialize this animation controller from
     * @param skeleton Root node of the skeleton of the transform tree to animation
     */
    AnimationController(const Json::Value& value, const Node *skeleton);
    
    /**
     * Do nothing constructor - just to handle meshes which don't have controllers
     */
    AnimationController();
    
    // Destroy an animation controller (sadness)
    ~AnimationController();
    
    /**
     * Bind a new skeleton to the animation controller
     * @param skeleton the root node of the skeleton to bind to the animation controller
     */
    void Bind(Node *root);
};

#endif
