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

#include "GLTransform.h"

// Standard constructor
GLTransform::GLTransform ()
    : translation(glm::vec3(0, 0, 0)), rotation(glm::quat()), scale(glm::vec3(1.0, 1.0, 1.0))
{
    
}

// Copy another transformation
GLTransform::GLTransform (const GLTransform& transform)
    : translation(transform.translation), rotation(transform.rotation), scale(transform.scale)
{
    
}

// Construct the transformation from a Json blob
GLTransform::GLTransform (const Json::Value& value)
    : translation(glm::vec3(0, 0, 0)), rotation(glm::quat()), scale(glm::vec3(1.0, 1.0, 1.0))
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
        rotation = glm::quat(value["rotation"][0].asFloat(), value["rotation"][1].asFloat(), value["rotation"][2].asFloat(), value["rotation"][3].asFloat());
    }
}

glm::vec3& GLTransform::Translation()
{
    return translation;
}

glm::vec3& GLTransform::Scale()
{
    return scale;
}

glm::quat& GLTransform::Rotation()
{
    return rotation;
}

// Return the transforms combined
glm::mat4 GLTransform::transformMatrix()
{
    //return glm::translate(translation) * glm::mat4_cast(rotation) * glm::scale(scale);
    return glm::scale(scale) * glm::mat4_cast(rotation) * glm::translate(translation);
}

