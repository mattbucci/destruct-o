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
        // All possible transform types
        typedef enum : unsigned char
        {
            kTransformTypeTranslation = 0,
            kTransformTypeScale = 1,
            kTransformTypeRotation = 2,
        } TransformType;

        // Iterator types
        typedef std::map<TransformType, const Transform>::iterator iterator;
        typedef std::map<TransformType, const Transform>::const_iterator const_iterator;

        // The time at which this keyframe is to trigger
        float keytime;

        // The transforms of they keyframe
        std::map<TransformType, const Transform> transforms;

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
