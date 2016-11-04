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

#ifndef __MODEL_GROUP_H__
#define __MODEL_GROUP_H__

#include "stdafx.h"
#include "Model.h"
#include "ModelInstance.h"
#include "TextureCache.h"
#include <mutex>

class ModelGroup
{
public:
    // Iterator types
    typedef std::map<std::string, ModelInstance *>::const_iterator const_iterator;
    typedef std::map<std::string, ModelInstance *>::size_type size_type;
private:
	//Protect the map while adding to the model group
	mutex modelGroupMutex;

    // The models in the model group
    std::map<std::string, ModelInstance *> models;

    // Texture cache for the models
    TextureCache&  textureCache;
public:
    // Create an empty model group
    ModelGroup(TextureCache& _textureCache);

    // Create a model group and load the models from the manifest
    ModelGroup(const std::string& manifestPath, TextureCache& _textureCache);

    // Insert a model into the model group (directory&path look it up, name is the lookup name)
    //void AddModel(std::string& directory, std::string& path, std::string& name);
    //void AddCompressedModel(std::string& directory, std::string& path, std::string& name);

    // Create a new model instance from a model in the group
    ModelInstance* NewInstance(const std::string modelName);

    // Get iterators
    const_iterator begin() const;
    const_iterator end() const;
    size_type size() const;
};

#endif
