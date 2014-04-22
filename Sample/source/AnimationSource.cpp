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
    : skeleton(new Node()), skeletonTable(Node::flattreemap()), initialSkeleton(new Node()), initialSkeletonTable(Node::const_flattreemap())
{
    
}

/**
 * Base copy constructor.  Copies the skeleton
 * @param source AnimationSource to duplicate
 */
AnimationSource::AnimationSource(const AnimationSource& source)
    : skeletonTable(Node::flattreemap()), initialSkeleton(source.initialSkeleton), initialSkeletonTable(source.initialSkeletonTable)
{
    // If there is a skeleton to duplicate
    if(source.skeleton)
    {
        // Duplicate the existing skeleton
        skeleton = new Node(*(source.skeleton));
    
        // Build the lookup table
        skeleton->GetFlatNodeTree(skeletonTable);
    }
}

/**
 * Base destructor, cleans up the local skeleton storage
 */
AnimationSource::~AnimationSource()
{
    // Cleanup the skeleton
    delete skeleton;
}

/**
 * Base assignment operator.  Animation source assumes the state of the other
 * @param controller Source to set this source to
 */
AnimationSource& AnimationSource::operator= (const AnimationSource& source)
{
    // Store the initial skeleton and store the lookup map
    initialSkeleton = source.initialSkeleton;
    initialSkeletonTable = source.initialSkeletonTable;
    skeletonTable = Node::flattreemap();
    
    // Purge previous state
    delete skeleton;
    
    // If we have a new skeleton, duplicate it
    if(source.skeleton)
    {
        // Create the local skeleton (a copy)
        skeleton = new Node(*(source.skeleton));
    
        // Generate the quick lookup tables of the skeletal components
        skeleton->GetFlatNodeTree(skeletonTable);
    }
    
    // Return a this pointer
    return *this;
}

/**
 * Binds the animation source to a skeleton
 * @param root the root node of the transform tree of the skeleton
 */
void AnimationSource::Bind(const Node* root)
{
    // Purge previous state
    initialSkeletonTable = Node::const_flattreemap();
    skeletonTable = Node::flattreemap();
    delete skeleton;
    
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
 * Get a reference to the local skeleton bone lookup map
 * @return reference to the local skeleton bone lookup map
 */
Node::flattreemap& AnimationSource::Bones()
{
    return skeletonTable;
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
 * Get a const reference to the local skeleton bone lookup map
 * @return const reference to the local skeleton bone lookup map
 */
const Node::flattreemap& AnimationSource::Bones() const
{
    return skeletonTable;
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
 * Get a const reference to the initial skeleton bone lookup map
 * @return const reference to the initial skeleton bone lookup map
 */
const Node::const_flattreemap& AnimationSource::InitialBones() const
{
    return initialSkeletonTable;
}
