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
#include "ModelInstance.h"
#include "OS.h"

/**
 * Create a model instance for a model
 * @param _model Model to provide an instance of
 */
ModelInstance::ModelInstance(Model *_model)
    : model(_model), skeleton(new Node(*(_model->Skeleton()))), transform(Transform()), animation(NULL), animationStartTime(0.0)
{
    
}

/**
 * Update the state of this model instance
 * @param delta time since last frame in seconds
 * @param now the current time
 */
void ModelInstance::Update(double delta, double now)
{
    // Make sure our model is uploaded
    model->Update(delta, now);
    
    // Update the skeleton of the model
    if(animation)
    {
        // Calculate the current animation time
        float animationTime = std::fmod(OS::Now() - animationStartTime, animation->Length());
        
        // Iterate through all the bones in the animation
        for (Animation::const_iterator bone = animation->begin(); bone != animation->end(); bone++)
        {
            // Lookup the node for this bone
            Node *node = skeleton->FindNode(bone->first);
            const Node *iNode = model->Skeleton()->FindNode(bone->first);
            
            // Iterate through the keyframes to select the bone
            for(std::vector<Animation::Keyframe *>::const_iterator keyframe = bone->second.begin(); keyframe != bone->second.end(); keyframe++)
            {
                // Get the next iterator
                std::vector<Animation::Keyframe *>::const_iterator nextKeyframe = keyframe + 1;
                
                // Is the animation time within the bounds of this frame
                if(nextKeyframe != bone->second.end())
                {
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
                
                /*else
                {
                    node->LocalTransform() = (*keyframe)->transform;
                    break;
                }*/
            }
        }
        
        // Recalculate the nodes
        skeleton->Recalculate();
    }
}

/**
 * Draw this model instance with a particular shader
 * @param program the shader to draw this model with
 */
void ModelInstance::Draw(MaterialProgram *program)
{
    // Push the model transform onto the program's matrix stack
    program->Model.PushMatrix();
    program->Model.SetMatrix(transform.TransformMatrix());
    program->Model.Apply();
    
    // Draw the model
    model->Draw(program, *skeleton);
    
    // Remove the translation
    program->Model.PopMatrix();
}

/**
 * Play an animation in this model by name
 * @param name the name of the animation to play
 * @return false if the animation did not exist, true if the animation is now playing
 */
bool ModelInstance::PlayAnimation(const std::string name)
{
    // Get an iterator to the animation we want to play
    Model::animation_const_iterator anim = model->Animations().find(name);
    
    // If the iterator is invalid, this failed, so escape
    if(anim == model->Animations().end())
    {
        return false;
    }
    
    // Store the animation object
    animation = anim->second;
    
    // Store the starting time of the animation
    animationStartTime = OS::Now();
    
    // Return success
    return true;
}

/**
 * Get a reference to the transform of the model
 * @return reference to transform
 */
Transform& ModelInstance::GetTransform()
{
    return transform;
}

const Model* ModelInstance::GetModel() const
{
    return model;
}

Node* ModelInstance::Skeleton()
{
    return skeleton;
}

const Node* ModelInstance::Skeleton() const
{
    return skeleton;
}

