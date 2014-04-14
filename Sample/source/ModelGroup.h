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

#ifndef __MODEL_GROUP_H__
#define __MODEL_GROUP_H__

#include "stdafx.h"
#include "Model.h"
#include "ModelInstance.h"
#include "TextureCache.h"

class ModelGroup
{
public:
    // Iterator types
    typedef std::map<std::string, Model *>::const_iterator const_iterator;
    typedef std::map<std::string, Model *>::size_type size_type;
private:
    // The models in the model group
    std::map<std::string, Model *> models;
    std::map<std::string, Transform> offsets;
    
    // Texture cache for the models
    TextureCache&  textureCache;
public:
    // Create an empty model group
    ModelGroup(TextureCache& _textureCache);
    
    // Create a model group and load the models from the manifest
    ModelGroup(const std::string& manifestPath, TextureCache& _textureCache);
    
    // Insert a model into the model group (directory&path look it up, name is the lookup name)
    void AddModel(std::string& directory, std::string& path, std::string& name);
    void AddCompressedModel(std::string& directory, std::string& path, std::string& name);

    // Create a new model instance from a model in the group
    ModelInstance* NewInstance(const std::string modelName);
    
    // Get iterators
    const_iterator begin() const;
    const_iterator end() const;
    size_type size() const;
};

#endif
