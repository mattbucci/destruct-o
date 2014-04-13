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

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "stdafx.h"
#include <json/json.h>

#include "Transform.h"

// Contains information about an animation in the mesh
class Animation
{
public:
    // Keyframe of an animation
    struct Keyframe
    {
        // The time at which this keyframe is to trigger
        float     keytime;
        
        // The transformation of this frame
        Transform transform;
        
        // Create the keyframe object from json
        Keyframe(const Json::Value& value);
    };
    
    // Iterator types
    typedef std::map<std::string, std::vector<Keyframe *> >::iterator iterator;
    typedef std::map<std::string, std::vector<Keyframe *> >::const_iterator const_iterator;
    
private:
    // A map of the bone names to the animation keyframes
    std::map<std::string, std::vector<Keyframe *> > boneKeyframes;
    
    // Name of the animation
    std::string id;
    
    // Length of the animation
    float       length;
    
public:
    // Create the animation object from json
    Animation();
    Animation(const Json::Value& value);
    ~Animation();
    
    // Name accessor methods
    std::string& Id();
    const std::string& Id() const;
    
    // Length accessor methods
    float& Length();
    const float& Length() const;
    
    // Iterator retrieval methods
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
};

#endif
