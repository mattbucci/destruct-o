/*
 *  Copyright 2014 Nathaniel Lewis, Matthew Bucci, Anthony Popelar, Brian Bamsch
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

#include "GLMesh.h"

// Include the JSON parser
#include <json/json.h>
#include <fstream>
#include "OS.h"

// Default constructor (loads all submeshes from model file)
GLMesh::GLMesh(const std::string& directory, const std::string& name, TextureCache& _textureCache, vec3 _scale)
    : vertexFrameSize(0), positionOffset(-1), normalOffset(-1), texCoordOffset(-1), textureCache(_textureCache), scale(_scale), currentAnimation(NULL)
{
    // Make sure blend weight offsets are in a "null" state
    for(int i = 0; i < MaxBoneWeights; i++) boneWeightOffset[i] = -1;
    
    // Open the Json file for reading
    std::string filename = directory + "/" + name;
    std::ifstream document;
    document.open(filename);
    
    // Create a json parser for this file
    Json::Reader reader;
    Json::Value  root;
    
    // Read the mesh file
    if(!reader.parse(document, root))
    {
        std::cerr << "Error parsing " << filename << std::endl;
        throw std::exception();
    }
    
    // Get the value for the mesh entry
    Json::Value& meshes = root["meshes"];
    
    // Sanity check on meshes, needs to be an array
    if(!meshes.isArray() || meshes.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": meshes entry in root is invalid" << std::endl;
        throw std::exception();
    }
    
    // Get the materials of the mesh
    Json::Value& mats = root["materials"];
    
    // Sanity check for the materials array
    if(!mats.isArray() || mats.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": materials entry in root is invalid" << std::endl;
        throw std::exception();
    }
    
    // Get the verticies array and upload it to the GPU
    Json::Value& vertices = meshes[0]["vertices"];
    
    // Sanity check for the verticies array
    if(!vertices.isArray() || vertices.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": vertices entry in mesh is invalid" << std::endl;
        throw std::exception();
    }
    
    // Get the parts of the mesh
    Json::Value& meshParts = meshes[0]["parts"];
    
    // Sanity check for the parts  array
    if(!meshParts.isArray() || meshParts.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": parts entry in mesh is invalid" << std::endl;
        throw std::exception();
    }
    
    // Get the shader properties of the mesh
    Json::Value& shaderAttributes = meshes[0]["attributes"];
    
    // Sanity check for the verticies array
    if(!shaderAttributes.isArray() || shaderAttributes.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": attributes entry in mesh is invalid" << std::endl;
        throw std::exception();
    }
    
    // Find the node that has the material identifiers
    Json::Value& nodes = root["nodes"];
    
    // Sanity check for the nodes array
    if(!nodes.isArray() || nodes.size() == 0)
    {
        std::cerr << "Error parsing " << filename << ": nodes entry in root is invalid" << std::endl;
        throw std::exception();
    }
    
    // Get the json blob as a float array (annoying, and slow, but because json value is an array of Json::Value, it is necessary
    std::vector<GLfloat> vertexData;
    for(Json::Value::iterator vIt = vertices.begin(); vIt != vertices.end(); vIt++)
    {
        vertexData.push_back((*vIt).asFloat());
    }
    
    // Create the vertex buffer for mesh data
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);
    
    // Iterate through the mesh parts
    for(Json::Value::iterator pIt = meshParts.begin(); pIt != meshParts.end(); pIt++)
    {
        // Create a new mesh part
        GLMesh::Part *part = new GLMesh::Part;
        
        // Get the indicies entry
        Json::Value& indices = (*pIt)["indices"];
        
        // Create the index buffer for this part
        std::vector<GLuint> indexData;
        for(Json::Value::iterator iIt = indices.begin(); iIt != indices.end(); iIt++)
        {
            indexData.push_back((*iIt).asUInt());
        }
        part->indexCount = indices.size();
        
        // Store the index buffer data
        glBindBuffer(GL_ARRAY_BUFFER, part->indexBuffer);
        glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indexData.data(), GL_STATIC_DRAW);
        
        // Store the mesh part
        parts[(*pIt)["id"].asString()] = part;
    }
    
    // Iterate through the shader properties
    for(Json::Value::iterator aIt = shaderAttributes.begin(); aIt != shaderAttributes.end(); aIt++)
    {
        // get the attribute as a string
        std::string attribute = (*aIt).asString();
        
        // Is this attribute the position of the vertex
        if(attribute == "POSITION")
        {
            // Store the offset for the position
            positionOffset = vertexFrameSize;
            vertexFrameSize += 3;
        }
        
        // Is this attribute the normal
        else if(attribute == "NORMAL")
        {
            // Store the offset for the normal
            normalOffset = vertexFrameSize;
            vertexFrameSize += 3;
        }
        
        // Is this attribute the texture coordinate
        else if(attribute.find("TEXCOORD") == 0)
        {
            // If this is the first texture coordinate
            attribute.erase(0, 8);
            GLuint idx = std::atoi(attribute.c_str());
            if(idx == 0)
            {
                // Store the offset for the texture coordinate
                texCoordOffset = vertexFrameSize;
            }
            
            // Always increment the frame, because we are trying to analyse the offsets
            vertexFrameSize += 2;
        }
        
        // Is this attribute the bone weight
        else if(attribute.find("BLENDWEIGHT") == 0)
        {
            // If this is the first texture coordinate
            attribute.erase(0, 11);
            GLuint idx = std::atoi(attribute.c_str());
            if(idx < MaxBoneWeights)
            {
                // Store the offset for the blend weight
                boneWeightOffset[idx] = vertexFrameSize;
            }
            
            // Always increment the frame, because we are trying to analyse the offsets
            vertexFrameSize += 2;
        }
        
        // Is this attribute the vertex color
        else if(attribute == "COLOR")
        {
            vertexFrameSize += 4;
        }
    }
    
    // Load materials
    for(Json::Value::iterator mIt = mats.begin(); mIt != mats.end(); mIt++)
    {
        // Get the textures entry of this material
        Json::Value& bitmaps = (*mIt)["textures"];
        
        // Loop through textures
        std::vector<std::string> textures;
        for(Json::Value::iterator tIt = bitmaps.begin(); tIt != bitmaps.end(); tIt++)
        {
            textures.push_back(directory + "/" + (*tIt)["filename"].asString());
            textureCache.GetTexture(textures.back());
        }
        
        // Store this material
        materials[(*mIt)["id"].asString()] = textures;
    }
    
    // Find the nodes with render steps
    for(Json::Value::iterator nIt = nodes.begin(); nIt != nodes.end(); nIt++)
    {
        // Check if we have a part member
        if((*nIt)["parts"] != Json::Value::null)
        {
            // Get the steps for this node
            Json::Value& steps = (*nIt)["parts"];
            
            // Iterate through the steps
            for(Json::Value::iterator sIt = steps.begin(); sIt != steps.end(); sIt++)
            {
                // Allocate a renderstep object
                RenderStep *renderStep = new RenderStep();
                
                // Store the mesh part id of this step
                renderStep->meshpartid = (*sIt)["meshpartid"].asString();
                
                // Store the material id of this step
                renderStep->materialid = (*sIt)["materialid"].asString();
                
                // Look through the bones
                if((*sIt)["bones"] != Json::Value::null)
                {
                    // Get the bones for this node
                    Json::Value& bones = (*sIt)["bones"];
                    
                    // Examine all the bones
                    for(Json::Value::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
                    {
                        // Construct a transform from the Json value
                        GLTransform transform = GLTransform(*bIt);
                        
                        // Store the bone
                        renderStep->bones.push_back(std::make_pair((*bIt)["node"].asString(), transform));
                    }
                }
                
                // Store the render step
                rendersteps.push_back(renderStep);
            }
        }
    }
    
    // Load the animations
    Json::Value &serializedAnimations = root["animations"];
    
    // Process the animations
    for(Json::Value::iterator aIt = serializedAnimations.begin(); aIt != serializedAnimations.end(); aIt++)
    {
        // Create a new animation object from the storage
        Animation *animation = new Animation(*aIt);
        
        // Store this animation
        animations[animation->name] = animation;
    }
    
    // Success
    std::cout << "Loaded mesh: \"" << filename.c_str() << "\"" << std::endl;
}

// Destroy this mesh
GLMesh::~GLMesh()
{
    // Iterate through all the submeshes
    for(std::map<std::string, Part *>::iterator it = parts.begin(); it != parts.end(); ++it)
    {
        delete it->second;
    }
    
    // Iterate through all rendersteps and free them
    for(std::vector<RenderStep *>::iterator it = rendersteps.begin(); it != rendersteps.end(); ++it)
    {
        delete *it;
    }
}

// Play an animation
void GLMesh::playAnimation(std::string name)
{
    // Store the animation object
    currentAnimation = animations[name];
    
    // Store the current time
    startTime = OS::Now();
}

// Draw this mesh
void GLMesh::Draw(GLMeshProgram *shader)
{
    // Iterate through all the submeshes
    for(std::vector<RenderStep *>::iterator it = rendersteps.begin(); it != rendersteps.end(); ++it)
    {
        // Get the part referenced here
        GLMesh::Part *part = parts[(*it)->meshpartid];
        
#if !(defined __ANDROID__)
        // Bind the vertex array object
        glBindVertexArray(part->attributes);

        // If the shader has changed, reload the vertex array object pointers
        if(shader != priorShader)
        {
            // Enable the vertex array object attributes that we will be using
            glEnableVertexAttribArray(shader->AttributeVertex());
            glEnableVertexAttribArray(shader->AttributeNormal());
            glEnableVertexAttribArray(shader->AttributeTexture());
            
#endif
            // Bind the vertex buffer for all our vertex attributes
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            
            // Set the vertex attribute pointer
            glVertexAttribPointer(shader->AttributeVertex(), 3, GL_FLOAT, GL_FALSE, vertexFrameSize * sizeof(GLfloat), (GLvoid *)(positionOffset * sizeof(GLfloat)));
            
            // Set the normal attribute pointer
            glVertexAttribPointer(shader->AttributeNormal(), 3, GL_FLOAT, GL_FALSE, vertexFrameSize * sizeof(GLfloat), (GLvoid *)(normalOffset * sizeof(GLfloat)));
            
            // Set the texture coordinate attribute pointer
            glVertexAttribPointer(shader->AttributeTexture(), 2, GL_FLOAT, GL_FALSE, vertexFrameSize * sizeof(GLfloat), (GLvoid *)(texCoordOffset * sizeof(GLfloat)));
            
            // Set the bone weights
            for(int i = 0; i < MaxBoneWeights; i++)
            {
                // Enable the vertex attribute array storage for
                glEnableVertexAttribArray(shader->AttributeBoneWeight(i));
                
                // Point the bone weight at the 
                glVertexAttribPointer(shader->AttributeBoneWeight(i), 2, GL_FLOAT, GL_FALSE, vertexFrameSize * sizeof(GLfloat), (GLvoid *)(boneWeightOffset[i] * sizeof(GLfloat)));
            }
            
            // Set the index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->indexBuffer);
            
#if !(defined __ANDROID__)
        }
#endif
        
        // Apply mesh scale
        shader->Model.PushMatrix();
        shader->Model.Scale(scale);
		shader->Model.Apply();
        shader->Model.PopMatrix();
        
        // Get the material
        std::vector<std::string>& textures = materials[(*it)->materialid];
        
        // Load and active the related textures
        GLuint textureUnit = GL_TEXTURE0;
        for(std::vector<std::string>::iterator tIt = textures.begin(); tIt != textures.end(); tIt++)
        {
            // Active the texture unit for a texture
            glActiveTexture(textureUnit++);
            
            // Bind the texture
            textureCache.GetTexture(*tIt)->Bind();
        }
        
        // Set the reflectivity
        glm::vec2 specular = glm::vec2(1.0, 1.0);
        glUniform2fv(glGetUniformLocation(shader->GetId(), "material_reflectivity"), 2, (const GLfloat *) &specular);
        
        // Push the bone data for this render stage
        for(int i = 0; i < (*it)->bones.size(); i++)
        {
            // Base bone state
            glm::mat4 bone = glm::mat4();
            
            // If we are currently playing an animation, potentially modify the bone
            if(currentAnimation)
            {
                // Lookup this bone in the animation
                std::map<std::string, std::vector<Keyframe *> >::iterator keyframes = currentAnimation->boneKeyframes.find((*it)->bones[i].first);
                
                // If this bone exists in the animation, update it
                if(keyframes != currentAnimation->boneKeyframes.end())
                {
                    // Get the current animation time
                    float animationTime = std::fmod(OS::Now() - startTime, currentAnimation->length);
                    
                    // Find the keyframe we want
                    for(std::vector<Keyframe *>::iterator kIt = keyframes->second.begin(); kIt != keyframes->second.end(); kIt++)
                    {
                        // Get the next iterator
                        std::vector<Keyframe *>::iterator nkIt = kIt + 1;
                        
                        // Is the animation time within the bounds of this frame
                        if(nkIt != keyframes->second.end())
                        {
                            if(animationTime < (*nkIt)->keytime)
                            {
                                bone = (*kIt)->transform.transformMatrix() /** glm::inverse((*it)->bones[i].second.transformMatrix())*/;
                                break;
                            }
                        }
                        
                        else
                        {
                            bone = (*kIt)->transform.transformMatrix() /** glm::inverse((*it)->bones[i].second.transformMatrix())*/;
                            break;
                        }
                    }
                }
            }
            
            // Push the bone to the GPU
            glUniformMatrix4fv(shader->UniformBones() + (i*4), 1, GL_FALSE, (const GLfloat *) &bone);
        }
        
        // Draw this mesh
        glDrawElements(GL_TRIANGLES, part->indexCount, GL_UNSIGNED_INT, 0);
    }
    
    // Store the new shader pointer
    priorShader = shader;
}

// Initialization of the submesh object
GLMesh::Part::Part()
{
#if !(defined __ANDROID__)
    // Generate a vertex array
    glGenVertexArrays(1, &attributes);
#endif
    
    // Generate the buffers for the submesh
    glGenBuffers(1, &indexBuffer);
}

// Deletion of the submesh object
GLMesh::Part::~Part()
{
#if !(defined __ANDROID__)
    // Generate a vertex array
    glDeleteVertexArrays(1, &attributes);
#endif
    // Delete the buffers
    glDeleteBuffers(1, &indexBuffer);
}

// Create a keyframe
GLMesh::Keyframe::Keyframe(const Json::Value& value)
{
    // Load the keytime
    keytime = value["keytime"].asFloat() / 1000.0f;
    
    // Load the transformaiton
    transform = GLTransform(value);
}

// Create an animation (it has been passed the animation's value)
GLMesh::Animation::Animation(const Json::Value& value)
{
    // Store the name of the animation
    name = value["id"].asString();
    
    // We need to iterate through the bones array
    const Json::Value& bones = value["bones"];
    
    // Process the bones
    for(Json::Value::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
    {
        // Get the name of the bone we are exploring
        std::string boneName = (*bIt)["boneId"].asString();
        
        // Create a vector for the keyframe
        std::vector<Keyframe *> keyframes;
        
        // Iterate through the keyframes
        const Json::Value& serializedKeyframes = (*bIt)["keyframes"];
        
        // Process the keyframes
        for(Json::Value::iterator kIt = serializedKeyframes.begin(); kIt != serializedKeyframes.end(); kIt++)
        {
            // Allocate a keyframe object from the serialized keyframe
            Keyframe *keyframe = new Keyframe(*kIt);
            
            // Store this keyframe in the vector
            keyframes.push_back(keyframe);
        }
        
        // Decide the length of the animation
        length = keyframes.size() * (keyframes[1]->keytime - keyframes[0]->keytime);
        
        // Store this bone
        boneKeyframes[boneName] = keyframes;
    }
}