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
#include "AnimationSource.h"

/**
 * Empty Constructor, initializes everything to a default state
 */
AnimationSource::AnimationSource()
    : skeleton(NULL), skeletonTable(Node::flattreemap()), initialSkeleton(NULL), initialSkeletonTable(Node::const_flattreemap())
{
    
}

/**
 * Base destructor, cleans up the local skeleton storage
 */
AnimationSource::~AnimationSource()
{
    // Cleanup the skeleton
    if(skeleton)
    {
        delete skeleton;
    }
}

/**
 * Binds the animation source to a skeleton
 * @param root the root node of the transform tree of the skeleton
 */
void AnimationSource::Bind(const Node* root)
{
    // Store the initial skeleton
    initialSkeleton = root;
    
    // Create the local skeleton (a copy)
    skeleton = new Node(*root);
    
    // Generate the quick lookup tables of the skeletal components
    initialSkeleton->GetFlatNodeTree(initialSkeletonTable);
    skeleton->GetFlatNodeTree(skeletonTable);
}

/**
 * Get a pointer to the local skeleton
 * @return pointer to the local skeleton
 */
Node* AnimationSource::Skeleton()
{
    return skeleton;
}

/**
 * Get a pointer to a named bone of the local skeleton
 * @return pointer to the named bone of the local skeleton
 */
Node* AnimationSource::Bone(const std::string& bone)
{
    // Attempt to find this bone
    Node::flattreemap::iterator b = skeletonTable.find(bone);
    
    // If we found the bone, return it
    if(b != skeletonTable.end())
    {
        return b->second;
    }
    
    // Otherwise return null for not found
    return NULL;
}

/**
 * Get a const pointer to the local skeleton
 * @return const pointer to the local skeleton
 */
const Node* AnimationSource::Skeleton() const
{
    return skeleton;
}

/**
 * Get a const pointer to a named bone of the local skeleton
 * @return const pointer to the named bone of the local skeleton
 */
const Node* AnimationSource::Bone(const std::string& bone) const
{
    // Attempt to find this bone
    Node::flattreemap::const_iterator b = skeletonTable.find(bone);
    
    // If we found the bone, return it
    if(b != skeletonTable.end())
    {
        return b->second;
    }
    
    // Otherwise return null for not found
    return NULL;
}

/**
 * Get a const pointer to the local skeleton
 * @return const pointer to the local skeleton
 */
const Node* AnimationSource::InitialSkeleton() const
{
    return initialSkeleton;
}

/**
 * Get a const pointer to a named bone of the local skeleton
 * @return const pointer to the named bone of the local skeleton
 */
const Node* AnimationSource::InitialBone(const std::string& bone) const
{
    // Attempt to find this bone
    Node::const_flattreemap::const_iterator b = initialSkeletonTable.find(bone);
    
    // If we found the bone, return it
    if(b != initialSkeletonTable.end())
    {
        return b->second;
    }
    
    // Otherwise return null for not found
    return NULL;
}
