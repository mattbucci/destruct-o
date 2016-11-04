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
