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

#ifndef __ANIMATION_CLIP_H__
#define __ANIMATION_CLIP_H__

#include "stdafx.h"

#include "AnimationController.h"
#include "AnimationSource.h"
#include "Animation.h"

/**
 * AnimationClip encapsulates the animation of a skeleton via
 * an Animation object loaded from a model
 */
class AnimationClip : public AnimationSource
{
    /** The keyframe animation that drives the animation clip */
    Animation *animation;
    
    /** The time at which the animation began playing */
    float animationStartTime;
    
    /** The time multiplier of the animation */
    float speed;
    
    /** Flag indicating whether the animation is playing */
    bool playing;
    
    /** Flag indicating whether the animation should play one-shot */
    bool loop;
public:
    /**
     * Standard constructor.  Initializes everything and stores the animation
     */
    AnimationClip(Animation *animation = NULL);
    
    /**
     * Copy constructor - duplicate another animation clip
     * @param animationClip the animation clip to duplicate
     */
    AnimationClip(const AnimationClip& animationClip);
    
    /**
     * Update the animation clip
     * @param delta time since last frame in seconds
     * @param now the current time
     */
    void Update(double delta, double now);
    
    /**
     * Setter method for the animation
     * @param animation the animation to play
     */
    void SetAnimation(Animation *animation);
    
    /**
     * Setter method for the animation playback speed
     * @param playback speed of the animation (1.0 for normal)
     */
    void SetPlaybackSpeed(float speed);
    
    /**
     * Begin playing the animation
     * @param loop should the animation play on a loop versus one shot
     * @param now the current simulated game time
     */
    void Play(bool loop, double now);
    
    /**
     * Stop the animation
     */
    void Stop();
    
    /**
     * Accessor method for the playing flag
     * @return true if the animation is playing
     */
    bool IsPlaying();
    
    /**
     * Accessor method for the loop flag
     * @return true if the animation is playing on a loop
     */
    bool IsLooping();
};

#endif
