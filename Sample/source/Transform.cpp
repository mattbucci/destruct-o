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
#include "Transform.h"

// Standard constructor
Transform::Transform ()
    : translation(glm::vec3(0, 0, 0)), rotation(glm::quat(1.0, 0.0, 0.0, 0.0)), scale(glm::vec3(1.0, 1.0, 1.0)), dirty(true)
{
    // Calculate the transform
    TransformMatrix();
}

// Copy another transformation
Transform::Transform (const Transform& transform)
    : translation(transform.translation), rotation(transform.rotation), scale(transform.scale), dirty(true)
{
    // Calculate the transform
    TransformMatrix();
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

    // Calculate the transform
    TransformMatrix();
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

    // Calculate the transform
    TransformMatrix();

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

// Interpolate between two transforms
Transform Transform::Interpolate(const Transform& a, const Transform&  b, const float t)
{
    // Final transform
    Transform f;

    // Compute the interpolated transformation
    f.Translation() = glm::mix(a.Translation(), b.Translation(), t);
    f.Scale() = glm::mix(a.Scale(), b.Scale(), t);
    f.Rotation() = glm::slerp(a.Rotation(), b.Rotation(), t);

    // Return the interpolated transform
    return f;
}
