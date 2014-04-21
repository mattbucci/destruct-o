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
 * modifications.  Layers and controllers are not sources, as they manage,
 * not provide animation data.  Examples of sources are animations and 
 * animation blend groups.
 *
 */
class AnimationSource
{
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
     * Base destructor, cleans up the local skeleton storage
     */
    virtual ~AnimationSource();
    
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
    virtual void Update(double delta, double now) = 0;
    
    /**
     * Get a pointer to the local skeleton
     * @return pointer to the local skeleton
     */
    Node* Skeleton();
    
    /**
     * Get a pointer to a named bone of the local skeleton
     * @return pointer to the named bone of the local skeleton
     */
    Node* Bone(const std::string& bone);
    
    /**
     * Get a const pointer to the local skeleton
     * @return const pointer to the local skeleton
     */
    const Node* Skeleton() const;
    
    /**
     * Get a const pointer to a named bone of the local skeleton
     * @return const pointer to the named bone of the local skeleton
     */
    const Node* Bone(const std::string& bone) const;
    
    /**
     * Get a const pointer to the initial skeleton
     * @return const pointer to the initial skeleton
     */
    const Node* InitialSkeleton() const;
    
    /**
     * Get a const pointer to a named bone of the initial skeleton
     * @return const pointer to the named bone of the initial skeleton
     */
    const Node* InitialBone(const std::string& bone) const;
};

#endif
