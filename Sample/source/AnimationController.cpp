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

// Unique parameter type count
const static unsigned int ParameterTypeCount = 2;

// Since deserialization is a feature, we need to have the parameter keys available
const static std::string ParameterTypeKeys[] =
{
    "float",
    "bool",
};

/**
 * Create an animation controller as a copy of another
 * @param value Json value to deserialize this animation controller from
 * @param skeleton Root node of the skeleton of the transform tree to animation
 */
AnimationController::AnimationController(const AnimationController& controller)
    : parameters(controller.parameters), layers(AnimationController::layer_store()), skeleton(NULL), initialSkeleton(NULL)
{
    // Duplicate the passed skeleton
    if(initialSkeleton && skeleton)
    {
        initialSkeleton = controller.initialSkeleton;
        skeleton = new Node(*(controller.skeleton));
    }
    
    // Duplicate the layers of the animation controller
    for(layer_const_iterator it = controller.layers.begin(); it != controller.layers.end(); it++)
    {
        layers[it->first] = new AnimationLayer(*(it->second));
    }
}

// Deserialize an animation controller
AnimationController::AnimationController(const Json::Value& value)
    : parameters(AnimationController::parameter_store()), layers(AnimationController::layer_store()), skeleton(NULL), initialSkeleton(NULL)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("AnimationController::AnimationController(const Json::Value& value) - value must be a object");
    }
    
    // We need to get all the parameters associated with this animation controller
    const Json::Value& serializedParameters = value["parameters"];
    
    // Iterate through the parameters and store them
    for(Json::Value::iterator parameter = serializedParameters.begin(); parameter != serializedParameters.end(); parameter++)
    {
        // Deserialize a parameter
        AnimationController::Parameter param = AnimationController::Parameter(*parameter);
        
        // Store this parameter
        parameters[param.name] = param;
    }
    
    // We need to get all the parameters associated with this animation controller
    const Json::Value& serializedLayers = value["layers"];
    
    // Iterate through the parameters and store them
    for(Json::Value::iterator layerIt = serializedLayers.begin(); layerIt != serializedLayers.end(); layerIt++)
    {
        // Deserialize a parameter
        AnimationLayer *layer = new AnimationLayer(*layerIt, *this);
        
        // Store the layer
        layers[layer->Id()] = layer;
    }
}

/**
 * Do nothing constructor - just to handle meshes which don't have controllers
 */
AnimationController::AnimationController()
    : parameters(AnimationController::parameter_store()), layers(AnimationController::layer_store()), skeleton(NULL), initialSkeleton(NULL)
{
    
}

/**
 * Destroy an animation controller and any heap allocated memory associated
 */
AnimationController::~AnimationController()
{
    // Iterate through the animation layers and DELETE them (mwhahah)
    for(layer_iterator it = layers.begin(); it != layers.end(); it++)
    {
        delete it->second;
    }
    
    // Delete the skeleton if we have one
    if(skeleton)
    {
        delete skeleton;
    }
}

/**
 * Assignment operator.  Animation controller assumes the state of the other
 * @param controller Controller to set this controller to
 */
AnimationController& AnimationController::operator= (const AnimationController& controller)
{
    // Copy the static resources
    parameters = controller.parameters;
    layers = layer_store();
    
    // Duplicate the skeletal model
    if(controller.initialSkeleton && controller.skeleton)
    {
        initialSkeleton = controller.initialSkeleton;
        skeleton = new Node(*(controller.skeleton));
    }
    
    // Duplicate the layers of the animation controller
    for(layer_const_iterator it = controller.layers.begin(); it != controller.layers.end(); it++)
    {
        AnimationLayer *layerCopy = new AnimationLayer(*(it->second), *this);
        layers[it->first] = layerCopy;
    }
    
    // Return a this pointer
    return *this;
}

/**
 * Update the animation controller.  Performs animation calculations and updates the skeleton
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationController::Update(double delta, double now)
{
    
    
    // For now just update everything to check
    for(layer_iterator it = layers.begin(); it != layers.end(); it++)
    {
        it->second->Update(delta, now);
    }
}

/**
 * Bind a new skeleton to the animation controller
 * @param skeleton the root node of the skeleton to bind to the animation controller
 */
void AnimationController::Bind(const Node *root)
{
    // Store the initial pose of the model for reference purposes
    initialSkeleton = root;
    
    // Create a duplicate node structure for the result of our animation
    skeleton = new Node(*root);
}

// Generic constructor to initialize the parameter
AnimationController::Parameter::Parameter()
    : name("null"), type(kTypeBool)
{
    value.boolean = false;
}

// Copy construtor
AnimationController::Parameter::Parameter(const Parameter& parameter)
    : name(parameter.name), type(parameter.type), value(parameter.value)
{
    
}

// Deserialization constructor
AnimationController::Parameter::Parameter(const Json::Value& value)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("AnimationController::Parameter::Parameter(const Json::Value& value) - value must be a object");
    }
    
    // Get the name of the parameter
    name = value["name"].asString();
    
    // Get the type
    const std::string *typeKey = std::find(&ParameterTypeKeys[0], &ParameterTypeKeys[0] + ParameterTypeCount, value["type"].asString());
    type = (AnimationController::Parameter::Type) std::distance(&ParameterTypeKeys[0], typeKey);
    
    // Store the value based on type
    switch (type)
    {
        case AnimationController::Parameter::kTypeFloat:
            this->value.number = value["value"].asFloat();
            break;
            
        case AnimationController::Parameter::kTypeBool:
            this->value.boolean = value["value"].asBool();
            break;
            
        default:
            throw std::runtime_error("AnimationController::Parameter::Parameter(const Json::Value& value) - unrecognized parameter type");
            break;
    };
}
