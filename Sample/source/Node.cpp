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


//Add all nodes (including the current one) to the given map
void Node::addAllNodes(std::map<std::string,Node *> & addTo) {
	addTo[id] = this;
	for (auto child : children)
		child->addAllNodes(addTo);
}


 // Construct a flattened map of all nodes (including this one) in the node tree
std::map<std::string,Node *> Node::AllNodes()
{
	std::map<std::string,Node *> nodes;
	addAllNodes(nodes);
	return nodes;
}