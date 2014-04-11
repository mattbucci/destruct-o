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

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "stdafx.h"
#include <json/json.h>

// The mesh is an object which
class Material
{
    // Name of the material
    std::string id;
public:
    // Standard constructor - create an empty material
    Material();
    
    // Deserialization constructor - load from a Json serialized blob
    Material(const Json::Value& value);
    
    // Get the name of the material
    std::string& Id();
};

#endif
