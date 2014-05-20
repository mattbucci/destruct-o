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
#include "Animation.h"

// Create an empty animation
Animation::Animation()
	: id(""), length(-numeric_limits<float>::infinity())
{
    
}

// Create the animation object from json
Animation::Animation(const Json::Value& value)
    : length(-numeric_limits<float>::infinity())
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Animation::Animation(const Json::Value& value) - value must be a object");
    }
    
    // Store the name of the animation
    id = value["id"].asString();
    
    // We need to iterate through the bones array
    const Json::Value& bones = value["bones"];
    
    // Process the bones
    for(Json::Value::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
    {
        // Get the name of the bone we are exploring
        std::string boneName = (*bIt)["boneId"].asString();
        
        // Create a vector for the keyframe
        std::vector<Keyframe *> keyframes;
        
        // Iterate through the keyframes
        const Json::Value& serializedKeyframes = (*bIt)["keyframes"];
        
        // Process the keyframes
        for(Json::Value::iterator kIt = serializedKeyframes.begin(); kIt != serializedKeyframes.end(); kIt++)
        {
            // Allocate a keyframe object from the serialized keyframe
            Keyframe *keyframe = new Keyframe(*kIt);
            
            // Store this keyframe in the vector
            keyframes.push_back(keyframe);
        }
        
        // Decide the length of the animation
        length = keyframes[keyframes.size() - 1]->keytime - keyframes[0]->keytime;
        
        // Store this bone
        boneKeyframes[boneName] = keyframes;
    }
}

// Destroy all heap allocated data
Animation::~Animation()
{
    // Iterate through all the keyframes and delete them
    for(iterator it = boneKeyframes.begin(); it != boneKeyframes.end(); it++)
    {
        for(std::vector<Keyframe *>::iterator kIt = it->second.begin(); kIt != it->second.end(); kIt++)
        {
            delete *kIt;
        }
    }
}

// Name accessor methods
std::string& Animation::Id()
{
    return id;
}

const std::string& Animation::Id() const
{
    return id;
}

// Length accessor methods
float& Animation::Length()
{
    return length;
}

const float& Animation::Length() const
{
    return length;
}

// Iterator retrieval methods
Animation::iterator Animation::begin()
{
    return boneKeyframes.begin();
}

Animation::const_iterator Animation::begin() const
{
    return boneKeyframes.begin();
}

Animation::iterator Animation::end()
{
    return boneKeyframes.end();
}

Animation::const_iterator Animation::end() const
{
    return boneKeyframes.end();
}

// Create a keyframe
Animation::Keyframe::Keyframe(const Json::Value& value)
{
    // Load the keytime
    keytime = value["keytime"].asFloat() / 1000.0f;
    
    // If a translation entry exists in the node
    if(value["translation"] != Json::Value::null)
    {
        // Load the translation from the Json blob
        transforms.insert(std::make_pair(Animation::Keyframe::kTransformTypeTranslation, Transform(value)));
    }
    
    // If a scale entry exists in the node
    if(value["scale"] != Json::Value::null)
    {
        // Load the translation from the Json blob
        transforms.insert(std::make_pair(Animation::Keyframe::kTransformTypeScale, Transform(value)));
    }
    
    // If a rotation entry exists in the node
    if(value["rotation"] != Json::Value::null)
    {
        // Load the translation from the Json blob
        transforms.insert(std::make_pair(Animation::Keyframe::kTransformTypeRotation, Transform(value)));
    }
}
