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
#include "Transform.h"

// Standard constructor
Transform::Transform ()
    : translation(glm::vec3(0, 0, 0)), rotation(glm::quat(1.0, 0.0, 0.0, 0.0)), scale(glm::vec3(1.0, 1.0, 1.0)), dirty(true)
{
    
}

// Copy another transformation
Transform::Transform (const Transform& transform)
    : translation(transform.translation), rotation(transform.rotation), scale(transform.scale), dirty(true)
{
    
}

// Construct the transformation from a Json blob
Transform::Transform (const Json::Value& value)
    : translation(glm::vec3(0, 0, 0)), rotation(glm::quat(1.0, 0.0, 0.0, 0.0)), scale(glm::vec3(1.0, 1.0, 1.0)), dirty(true)
{
    // If a translation entry exists in the node
    if(value["translation"] != Json::Value::null)
    {
        // Load the translation from the Json blob
        translation = glm::vec3(value["translation"][0].asFloat(), value["translation"][1].asFloat(), value["translation"][2].asFloat());
    }
    
    // If a scale entry exists in the node
    if(value["scale"] != Json::Value::null)
    {
        // Load the scale transform
        scale = glm::vec3(value["scale"][0].asFloat(), value["scale"][1].asFloat(), value["scale"][2].asFloat());
    }
    
    // If a rotation entry exists in the node
    if(value["rotation"] != Json::Value::null)
    {
        rotation = glm::quat(value["rotation"][3].asFloat(), value["rotation"][0].asFloat(), value["rotation"][1].asFloat(), value["rotation"][2].asFloat());
    }
}

// Assignment operator (copy the contents of the other transform)
Transform& Transform::operator=(const Transform &transform)
{
    // Copy the contents
    translation = transform.translation;
    rotation    = transform.rotation;
    scale       = transform.scale;
    
    // Contents are now dirty
    dirty = true;
    
    // Return a reference to ourself
    return *this;
}

// Return read only references to our internal content (does NOT flag content as dirty)
const glm::vec3& Transform::Translation() const
{
    return translation;
}

const glm::vec3& Transform::Scale() const
{
    return scale;
}

const glm::quat& Transform::Rotation() const
{
    return rotation;
}

// Return read/write references to our internal content (flags contents as dirty)
glm::vec3& Transform::Translation()
{
    dirty = true;
    return translation;
}

glm::vec3& Transform::Scale()
{
    dirty = true;
    return scale;
}

glm::quat& Transform::Rotation()
{
    dirty = true;
    return rotation;
}

// Return the transforms combined
const glm::mat4& Transform::TransformMatrix()
{
    // If the transformation is dirty update it
    if(dirty)
    {
        // Update the transformation matrix
        transformMatrix = glm::translate(translation) * glm::mat4_cast(rotation) * glm::scale(scale);
        
        // Clear the dirty flag
        dirty = false;
    }
    
    // Return the transformation matrix
    return transformMatrix;
}

// Return the transforms combined
const glm::mat4& Transform::TransformMatrix() const
{
    // If the transformation is dirty update it
    if(dirty)
    {
        throw new std::runtime_error("const glm::mat4& Transform::TransformMatrix() const - attempted to read a dirty transformation matrix");
    }
    
    // Return the transformation matrix
    return transformMatrix;
}

