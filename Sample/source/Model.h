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

#ifndef __MODEL_H__
#define __MODEL_H__

#include "stdafx.h"

#include "VertexAttributes.h"
#include "Mesh.h"
#include "Transform.h"
#include "Node.h"

// The mesh is an object which
class Model
{
public:
    // Meshes belonging to this model
    std::vector<Mesh *>       meshes;
    
    // Root node of the model
    Node                     *node;
    
public:
    // Create an empty model for population manually
    Model();
    
    // Create a model by deserializing it from a json file
    Model(const std::string& directory, const std::string& name);
    
    // Standard deconstructor - free all heap allocated data
    ~Model();
    
};

#endif
