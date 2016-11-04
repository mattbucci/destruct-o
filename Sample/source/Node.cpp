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
#include "Node.h"

// Construct an empty node
Node::Node()
    : id(""), globalTransformMatrix(glm::mat4()), transform(Transform()), children(0, NULL), parent(NULL)
{
    // Initialize all internal transforms
    Recalculate();
}

// Construct a node as a copy of another node (will not be in the heirarchy)
Node::Node(const Node& node, Node *_parent)
    : id(node.id), globalTransformMatrix(node.TransformMatrix()), transform(node.transform), children(0, NULL), parent(_parent)
{
    // Iterate through the children of this node and add them as new nodes
    for(std::vector<Node *>::const_iterator it = node.Children().begin(); it != node.Children().end(); it++)
    {
        // Create a duplicate node of the child
        Node *child = new Node(**it, this);

        // This is as a child to us
        children.push_back(child);
    }
}

// Construct the node from a Json blob
Node::Node(const Json::Value& value, Node *_parent)
    : globalTransformMatrix(glm::mat4()), children(0, NULL), parent(_parent)
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Node::Node(const Json::Value& value) - value must be a object");
    }

    // Get the id from the node
    id = value["id"].asString();

    // Load the serialized transform (possibly)
    transform = Transform(value);

    // Recalculate the node to setup our matrices
    Recalculate();

    // If this node comes with children, add them
    const Json::Value& childs = value["children"];
    if(childs != Json::Value::null)
    {
        for(Json::Value::iterator it = childs.begin(); it != childs.end(); it++)
        {
            // Create a new child node
            Node *child = new Node(*it, this);

            // Add this child to our list
            children.push_back(child);
        }
    }
}

// Deallocate all heap memory
Node::~Node()
{
    // Iterate through all meshes and free the data
    for(std::vector<Node *>::iterator it = children.begin(); it != children.end(); it++)
    {
        delete *it;
    }
}

std::string& Node::Id()
{
    return id;
}

const std::string& Node::Id() const
{
    return id;
}

const glm::mat4& Node::TransformMatrix() const
{
    return globalTransformMatrix;
}

// Get a read/write reference to the transform
Transform& Node::LocalTransform()
{
    return transform;
}

// Get a read only reference to the transform
const Transform& Node::LocalTransform() const
{
    return transform;
}

// Cause this node to recalculate
void Node::Recalculate()
{
    // Update our global transform before the children, else all hell breaks loose
    if(parent)
    {
        // Recalculate our global transform matrix
        globalTransformMatrix = parent->globalTransformMatrix * transform.TransformMatrix();
    }

    // If we don't have a parent, the global transform is just our transform
    else
    {
        globalTransformMatrix = transform.TransformMatrix();
    }

    // Cause the children to recalculate
    for(std::vector<Node *>::iterator it = children.begin(); it != children.end(); it++)
    {
        (*it)->Recalculate();
    }
}

// Add a child node to this node (reparent, recalculate, add to local storage)
void Node::AddChild(Node *child, bool recalculate)
{
    // Reparent this child to us
    child->parent = this;

    // Cause the child to recalculate (if desired)
    if(recalculate)
    {
        child->Recalculate();
    }

    // Add to children list
    children.push_back(child);
}

// Find a node by name
Node* Node::FindNode(const std::string& name)
{
    // If we are the desired node, return ourself
    if(id == name)
    {
        return this;
    }

    // Otherwise search in the children
    Node *node = NULL;
    for(std::vector<Node *>::iterator it = children.begin(); it != children.end(); it++)
    {
        // Search in this child
        node = (*it)->FindNode(name);

        // If we found the node, get out
        if(node)
        {
            break;
        }
    }

    // Return null if we fail to find the node
    return node;
}

// Find a node by name (constant)
const Node* Node::FindNode(const std::string &name) const
{
    // If we are the desired node, return ourself
    if(id == name)
    {
        return this;
    }

    // Otherwise search in the children
    const Node *node = NULL;
    for(std::vector<Node *>::const_iterator it = children.begin(); it != children.end(); it++)
    {
        // Search in this child
        node = (*it)->FindNode(name);

        // If we found the node, get out
        if(node)
        {
            break;
        }
    }

    // Return the found node
    return node;
}

// Get a reference to the children of the node
const std::vector<Node *>& Node::Children() const
{
    return children;
}

// Construct a flattened map of all nodes (including this one) in the node tree
void Node::GetFlatNodeTree(Node::flattreemap& table)
{
    // Add this node to the tree
    table[id] = this;

    // Iterate through the children and add them to the table
    for(std::vector<Node *>::iterator child = children.begin(); child != children.end(); child++)
    {
        // Flatten the child
        (*child)->GetFlatNodeTree(table);
    }
}

// Construct a flattened map of all nodes (including this one) in the node tree (const pointers)
void Node::GetFlatNodeTree(Node::const_flattreemap& table) const
{
    // Add this node to the tree
    table[id] = this;

    // Iterate through the children and add them to the table
    for(std::vector<Node *>::const_iterator child = children.begin(); child != children.end(); child++)
    {
        // Flatten the child
        (*child)->GetFlatNodeTree(table);
    }
}
