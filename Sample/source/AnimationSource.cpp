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
 * Performs an update of the animation source
 * @param delta the elapsed time since the previous frame
 * @param now the current simulated time
 */
void AnimationSource::Update(double delta, double now)
{

}

/**
 * Sets the local skeleton's transforms back to the initial pose
 */
void AnimationSource::Reset()
{
    // Copy the initial pose bones back into the local skeleton
    for(Node::const_flattreemap::const_iterator it = InitialBones().begin(); it != InitialBones().end(); it++)
    {
        skeletonTable[it->first]->LocalTransform() = it->second->LocalTransform();
    }
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
