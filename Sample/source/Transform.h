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

#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "stdafx.h"
#include <json/json.h>

// Class to wrap rotation, translation, and scale transformations
class Transform
{
    // Translation
    glm::vec3 translation;

    // Rotation
    glm::quat rotation;

    // Scale
    glm::vec3 scale;

    // Current transform matrix
    glm::mat4 transformMatrix;

    // Dirty flag (needs to re-transform)
    bool      dirty;
public:
    // Standard constructors
    Transform();
    Transform(const Transform&   transform);
    Transform(const Json::Value& value);

    // Assignment operator
    Transform& operator= (const Transform& transform);

    // Read only references to members, does not set dirty flag (causes recalculation)
    const glm::vec3& Translation() const;
    const glm::vec3& Scale() const;
    const glm::quat& Rotation() const;

    // Read/write references to members, sets a dirty flag
    glm::vec3&       Translation();
    glm::vec3&       Scale();
    glm::quat&       Rotation();

    // Get a combined transformation matrix
    const glm::mat4& TransformMatrix();

    // Get a combined transformation matrix
    const glm::mat4& TransformMatrix() const;

    // Interpolate between two transforms
    static Transform Interpolate(const Transform& a, const Transform&  b, const float t);
};

#endif
