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
    const Animation *animation;

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
    AnimationClip(const Animation *animation = NULL);

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
    void SetAnimation(const Animation *animation);

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

    /**
     * Method to get current animation progress
     * @return animation progress (0.0 -> 1.0)
     */
    const float GetProgress(double now) const;
};

#endif
