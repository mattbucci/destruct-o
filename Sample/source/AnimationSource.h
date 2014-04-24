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

#ifndef __ANIMATION_SOURCE_H__
#define __ANIMATION_SOURCE_H__

#include "stdafx.h"
#include "Node.h"

/**
 * Animation source is a base class for all objects which provide skeletal
 * modifications.
 */
class AnimationSource
{
protected:
    /** The local storage of the target skeleton */
    Node *skeleton;
    
    /** The fast lookup table for the local skeleton */
    Node::flattreemap skeletonTable;
    
    /** A read-only reference to the initial skeletal pose */
    const Node *initialSkeleton;
    
    /** The fast lookup table for the initial skeletal pose */
    Node::const_flattreemap initialSkeletonTable;
    
public:
    /**
     * Base constructor, initializes everything to a default state
     */
    AnimationSource();
    
    /**
     * Base copy constructor.  Copies the skeleton
     * @param source AnimationSource to duplicate
     */
    AnimationSource(const AnimationSource& source);
    
    /** 
     * Base destructor, cleans up the local skeleton storage
     */
    virtual ~AnimationSource();
    
    /**
     * Base assignment operator.  Animation source assumes the state of the other
     * @param controller Source to set this source to
     */
    AnimationSource& operator= (const AnimationSource& source);
    
    /**
     * Binds the animation source to a skeleton
     * @param root the root node of the transform tree of the skeleton
     */
    virtual void Bind(const Node* root);
    
    /**
     * Performs an update of the animation source
     * @param delta the elapsed time since the previous frame
     * @param now the current simulated time
     */
    virtual void Update(double delta, double now);
    
    /**
     * Sets the local skeleton's transforms back to the initial pose
     */
    virtual void Reset();
    
    /**
     * Get a pointer to the local skeleton
     * @return pointer to the local skeleton
     */
    Node* Skeleton();
    
    /**
     * Get a reference to the local skeleton bone lookup map
     * @return reference to the local skeleton bone lookup map
     */
    Node::flattreemap& Bones();
    
    /**
     * Get a const pointer to the local skeleton
     * @return const pointer to the local skeleton
     */
    const Node* Skeleton() const;
    
    /**
     * Get a const reference to the local skeleton bone lookup map
     * @return const reference to the local skeleton bone lookup map
     */
    const Node::flattreemap& Bones() const;
    
    /**
     * Get a const pointer to the initial skeleton
     * @return const pointer to the initial skeleton
     */
    const Node* InitialSkeleton() const;
    
    /**
     * Get a const reference to the initial skeleton bone lookup map
     * @return const reference to the initial skeleton bone lookup map
     */
    const Node::const_flattreemap& InitialBones() const;
};

#endif
