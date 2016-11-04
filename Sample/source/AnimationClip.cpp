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
#include "AnimationClip.h"

/**
 * Standard constructor.  Initializes everything and stores the animation
 */
AnimationClip::AnimationClip(const Animation *_animation)
    : AnimationSource(), animation(_animation), animationStartTime(0.0f), speed(1.0f), playing(false), loop(false)
{

}

/**
 * Copy constructor - duplicate another animation clip
 * @param animationClip the animation clip to duplicate
 */
AnimationClip::AnimationClip(const AnimationClip& animationClip)
    : AnimationSource(animationClip), animation(animationClip.animation), animationStartTime(animationClip.animationStartTime), speed(animationClip.speed), playing(animationClip.playing), loop(animationClip.loop)
{

}

/**
 * Setter method for the animation
 * @param _animation the animation to play
 */
void AnimationClip::SetAnimation(const Animation *_animation)
{
    animation = _animation;
}

/**
 * Setter method for the animation playback speed
 * @param playback speed of the animation (1.0 for normal)
 */
void AnimationClip::SetPlaybackSpeed(float _speed)
{
    speed = _speed;
}

/**
 * Begin playing the animation
 * @param _loop should the animation play on a loop versus one shot
 * @param now the current simulated game time
 */
void AnimationClip::Play(bool _loop, double now)
{
    // Start playing
    playing = true;
    loop = _loop;

    // Store the starting time
    animationStartTime = (float)now;

    // Force an update cycle.  Will set animation to the first frame of the animation
    Update(0.01, now);
}

/**
 * Stop the animation
 */
void AnimationClip::Stop()
{
    // Stop playback
    playing = false;
}

/**
 * Accessor method for the playing flag
 * @return true if the animation is playing
 */
bool AnimationClip::IsPlaying()
{
    return playing;
}

/**
 * Accessor method for the loop flag
 * @return true if the animation is playing on a loop
 */
bool AnimationClip::IsLooping()
{
    return loop;
}

/**
 * Update the animation clip
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void AnimationClip::Update(double delta, double now)
{
    // Update the skeleton of the model
    if(animation && playing)
    {
        // Calculate the current animation time
        float animationTime = (float)((now - animationStartTime) * speed);

        // Make sure its not negative
        if(animationTime < 0)
        {
            animationStartTime = (float)now;
            animationTime = 0.0;
        }

        // If the animation time is greater than the length of the animation and we are not looping
        else if(animationTime >= animation->Length() && !loop)
        {
            playing = false;
            return;
        }

        // Otherwise just wrap the animation time
        else
        {
            animationTime = std::fmod(animationTime, animation->Length());
        }

        // Iterate through all the bones in the animation
        for (Animation::const_iterator bone = animation->begin(); bone != animation->end(); bone++)
        {
            // Lookup the node for this bone
            Node *node = skeletonTable[bone->first];
            const Node *iNode = initialSkeletonTable[bone->first];

            // Error if we could not find the bones (skip bone)
            if(!node || !iNode)
            {
                //cout << "WTF: could not find bone -> " << bone->first << endl;
                continue;
            }

            // Iterate through the keyframes to select the bone
            for(std::vector<Animation::Keyframe *>::const_iterator keyframe = bone->second.begin(); keyframe != bone->second.end(); keyframe++)
            {
                // Get the next iterator
                std::vector<Animation::Keyframe *>::const_iterator nextKeyframe = keyframe + 1;

                // If the next keyframe is the end of the animation
                if(nextKeyframe == bone->second.end())
                {
                    // Point the next keyframe at the beginning
                    nextKeyframe = bone->second.begin();
                }

                // Update the animation (blend between keyframes)
                if(animationTime < (*nextKeyframe)->keytime)
                {
                    // Calculate position between the two keyframes
                    const float t = (animationTime - (*keyframe)->keytime) / ((*nextKeyframe)->keytime - (*keyframe)->keytime);

                    // Get the two translations to lerp between (they default to the initial pose)
                    glm::vec3 tA = iNode->LocalTransform().Translation();
                    glm::vec3 tB = tA;
                    Animation::Keyframe::iterator itA = (*keyframe)->transforms.find(Animation::Keyframe::kTransformTypeTranslation);
                    Animation::Keyframe::iterator itB = (*nextKeyframe)->transforms.find(Animation::Keyframe::kTransformTypeTranslation);
                    if(itA != (*keyframe)->transforms.end())
                    {
                        tA = itA->second.Translation();
                    }
                    if(itB != (*nextKeyframe)->transforms.end())
                    {
                        tB = itB->second.Translation();
                    }

                    // Get the two scales to lerp between (they default to the initial pose)
                    glm::vec3 sA = iNode->LocalTransform().Scale();
                    glm::vec3 sB = sA;
                    Animation::Keyframe::iterator isA = (*keyframe)->transforms.find(Animation::Keyframe::kTransformTypeScale);
                    Animation::Keyframe::iterator isB = (*nextKeyframe)->transforms.find(Animation::Keyframe::kTransformTypeScale);
                    if(isA != (*keyframe)->transforms.end())
                    {
                        sA = isA->second.Scale();
                    }
                    if(isB != (*nextKeyframe)->transforms.end())
                    {
                        sB = isB->second.Scale();
                    }

                    // Get the two rotations to slerp between (they default to the initial pose)
                    glm::quat rA = iNode->LocalTransform().Rotation();
                    glm::quat rB = rA;
                    Animation::Keyframe::iterator irA = (*keyframe)->transforms.find(Animation::Keyframe::kTransformTypeRotation);
                    Animation::Keyframe::iterator irB = (*nextKeyframe)->transforms.find(Animation::Keyframe::kTransformTypeRotation);
                    if(irA != (*keyframe)->transforms.end())
                    {
                        rA = irA->second.Rotation();
                    }
                    if(irB != (*nextKeyframe)->transforms.end())
                    {
                        rB = irB->second.Rotation();
                    }

                    // Set the skeleton node properly
                    node->LocalTransform().Translation() = glm::mix(tA, tB, t);
                    node->LocalTransform().Scale() = glm::mix(sA, sB, t);
                    node->LocalTransform().Rotation() = glm::slerp(rA, rB, t);
                    break;
                }
            }
        }

        // Recalculate the nodes
        skeleton->Recalculate();
    }
}

/**
 * Method to get current animation progress
 * @return animation progress (0.0 -> 1.0)
 */
const float AnimationClip::GetProgress(double now) const
{
    // If the animation is not playing, it is "done"
    if(!playing)
        return 1.0f;

    // Calculate the current animation time
    float animationTime = (float)((now - animationStartTime) * speed);

    // Make sure its not negative
    if(animationTime < 0)
    {
        animationTime = 0.0;
    }

    // If the animation is capable of looping, do so
    else
    {
        animationTime = std::fmod(animationTime, animation->Length());
    }

    // Return the progress
    return animationTime / animation->Length();
}
