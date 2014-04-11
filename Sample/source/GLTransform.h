/*
 *  Copyright 2014 Nathaniel Lewis, Matthew Bucci, Anthony Popelar, Brian Bamsch
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

#ifndef __GL_TRANSFORM_H__
#define __GL_TRANSFORM_H__

#include "stdafx.h"
#include <json/json.h>

// Class to wrap rotation, translation, and scale transformations
class GLTransform
{
    // Translation
    glm::vec3 translation;
    
    // Rotation
    glm::quat rotation;
    
    // Scale
    glm::vec3 scale;
public:
    // Standard constructors
    GLTransform();
    GLTransform(const GLTransform& transform);
    GLTransform(const Json::Value& value);
    
    // Get references to the members
    glm::vec3& Translation();
    glm::vec3& Scale();
    glm::quat& Rotation();
    
    // Get a combined transformation matrix
    glm::mat4  transformMatrix();
};

#endif