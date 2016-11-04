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

#ifndef __NODE_H__
#define __NODE_H__

#include "stdafx.h"
#include <json/json.h>

#include "Transform.h"

// Class to wrap rotation, translation, and scale transformations
class Node
{
public:
    // Flat tree map types
    typedef std::map<std::string, Node *> flattreemap;
    typedef std::map<std::string, const Node *> const_flattreemap;

private:
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

    // Construct a node as a copy of another node (clones whole tree, cannot parent)
    Node(const Node& node, Node *parent = NULL);

    // Construct the node from a Json blob
    Node(const Json::Value& value, Node *parent = NULL);

    // Standard deconstructor - deallocate all heap memory
    ~Node();

    // Get the name of the node
    std::string& Id();
    const std::string& Id() const;

    // Get a reference to the internal transformation matrix
    const glm::mat4& TransformMatrix() const;

    // Get a read/write reference to the transform
    Transform& LocalTransform();

    // Get a read only reference to the transform
    const Transform& LocalTransform() const;

    // Cause this node to recalculate, along with all the child nodes
    void Recalculate();

    // Add a child node to this node
    void AddChild(Node *child, bool recalculate = true);

    // Find a child node by name in this node heirarchy
    Node* FindNode(const std::string& name);

    // Find a child node by name in this node heirarchy (const function)
    const Node* FindNode(const std::string& name) const;

    // Get a reference to the children of the node
    const std::vector<Node *>& Children() const;

    // Construct a flattened map of all nodes (including this one) in the node tree
    void GetFlatNodeTree(flattreemap& table);
    void GetFlatNodeTree(const_flattreemap& table) const;
};

#endif
