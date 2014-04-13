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
};

#endif