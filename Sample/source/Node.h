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

#ifndef __NODE_H__
#define __NODE_H__

#include "stdafx.h"
#include <json/json.h>

#include "Transform.h"

// Class to wrap rotation, translation, and scale transformations
class Node
{
    // Name of the node
    std::string          id;
    
    // The global transformation matrix
    glm::mat4            globalTransformMatrix;
    
    // The transform of this node
    Transform            transform;
    
    // This node's children
    std::vector<Node *>  children;
    
    // This node's parent
    Node                *parent;
    
public:
    // Construct an empty node
    Node();
    
    // Construct the node from a Json blob
    Node(const Json::Value& value, Node *parent = NULL);
    
    // Standard deconstructor - deallocate all heap memory
    ~Node();
    
    // Get the name of the node
    std::string       Id();
    
    // Get a read only reference to the transform
    const glm::mat4&  TransformMatrix();
    
    // Get a read/write reference to the transform
    Transform&        LocalTransform();
    
    // Cause this node to recalculate, along with all the child nodes
    void              Recalculate();
    
    // Add a child node to this node
    void              AddChild(Node *child, bool recalculate = true);
    
    // Find a child node by name in this heirchy
    Node*             FindNode(const std::string& name);
    
    // Get a reference to the children of the node
    const std::vector<Node *>& Children();
};

#endif