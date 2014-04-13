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

#include "Model.h"
#include "OS.h"

#include <fstream>

// Create an empty model for population manually
Model::Model(TextureCache &_textureCache)
    : meshes(0, NULL), skeleton(new Node()), textureCache(_textureCache), previousProgram(NULL), uploaded(false)
{
    
}

// Create a model by deserializing it from Json
Model::Model(const std::string& directory, const std::string& name, TextureCache &_textureCache)
    : Model::Model(_textureCache)
{
    // Open the Json file for reading
    std::string filename = directory + "/" + name;
    std::ifstream document;
    document.open(filename);
    
    // Create a json parser for this file
    Json::Reader reader;
    Json::Value  root;
    
    // Read the model file
    if(!reader.parse(document, root))
    {
        std::cerr << "Error parsing " << filename << std::endl;
        throw std::exception();
    }
    
    // Load animations
    loadAnimations(root);
    
    // Load materials
    loadMaterials(root, directory);
    
    // Load mesh data
    loadMeshes(root);
    
    // Load node data
    loadSkeleton(root);
    
    // Load parts
    loadParts(root);
}

Model::~Model()
{
    // Iterate through all meshes and free the data
    for(std::vector<Mesh *>::iterator it = meshes.begin(); it != meshes.end(); it++)
    {
        delete *it;
    }
    
    // Destroy all of the vertex buffers associated with meshes
    for(std::map<Mesh *, GLuint>::iterator it = buffers.begin(); it != buffers.end(); it++)
    {
        // If the contents are still a valid vertex buffer
        if(glIsBuffer(it->second) == GL_TRUE)
        {
            glDeleteBuffers(1, &it->second);
        }
    }
    
    // Destroy all of the part render objects
    for(std::vector<Model::MeshPartRenderData *>::iterator it = renderables.begin(); it != renderables.end(); it++)
    {
        // Delete the index buffer
        if(glIsBuffer((*it)->indices) == GL_TRUE)
        {
            glDeleteBuffers(1, &(*it)->indices);
        }
#if !(defined __ANDROID__)
        if(glIsVertexArray((*it)->attributes) == GL_TRUE)
        {
            glDeleteVertexArrays(1, &(*it)->attributes);
        }
#endif
        
        // Delete the object itself
        delete *it;
    }
    
    // Destroy all of the materials
    for(std::map<std::string, Material *>::iterator it = materials.begin(); it != materials.end(); it++)
    {
        delete it->second;
    }

    // Destroy all of the animations
    for(std::map<std::string, Animation *>::iterator it = animations.begin(); it != animations.end(); it++)
    {
        delete it->second;
    }
    
    // Destroy the skeletal nodes
    delete skeleton;
}

// Helper function to load animations form the model
void Model::loadAnimations(const Json::Value &root)
{
    // Load the animations
    const Json::Value &serializedAnimations = root["animations"];
    
    // If the model file contains animation entries
    if(serializedAnimations != Json::Value::null)
    {
        // Process the animations
        for(Json::Value::iterator it = serializedAnimations.begin(); it != serializedAnimations.end(); it++)
        {
            // Create a new animation object from the storage
            Animation *animation = new Animation(*it);
            
            // Store this animation
            animations[animation->Id()] = animation;
        }
    }
}

// Helper function to load the materials from the serialized Json blob
void Model::loadMaterials(const Json::Value &root, const std::string& directory)
{
    // Get the value for the mesh entry
    const Json::Value& mats = root["materials"];
    
    // If the model file contains mesh entries
    if(mats != Json::Value::null)
    {
        // Iterate through all the mesh entries
        for(Json::Value::iterator it = mats.begin(); it != mats.end(); it++)
        {
            // Load a material from the entry
            Material *material = new Material(*it, directory);
            
            // Store this material for later usage
            materials[material->Id()] = material;
        }
    }
}

// Helper function to load the mesh data from the serialized Json blob
void Model::loadMeshes(const Json::Value& root)
{
    // Get the value for the mesh entry
    const Json::Value& meshes = root["meshes"];
    
    // If the model file contains mesh entries
    if(meshes != Json::Value::null)
    {
        // Iterate through all the mesh entries
        for(Json::Value::iterator it = meshes.begin(); it != meshes.end(); it++)
        {
            // Load a mesh from the json blob
            Mesh *mesh = new Mesh(*it);
            
            // Hold on to the mesh
            this->meshes.push_back(mesh);
        }
    }
}

// Helper function to load the node tree from the serialized json data
void Model::loadSkeleton(const Json::Value &root)
{
    // Get potential node entries
    const Json::Value& nodes = root["nodes"];
    
    // If the model contains node entries
    if(nodes != Json::Value::null)
    {
        // Create the node heirchy before search for mesh rendering information
        for(Json::Value::iterator it = nodes.begin(); it != nodes.end(); it++)
        {
            // Create a new child node
            Node *child = new Node(*it, skeleton);
            
            // Add this child to our list
            skeleton->AddChild(child, false);
        }
    }
}

// Helper function to load the parts from the serialized json data
void Model::loadParts(const Json::Value &root)
{
    // Get potential node entries
    const Json::Value& nodes = root["nodes"];
    
    // If the model contains node entries
    if(nodes != Json::Value::null)
    {
        // Loop through all the nodes and check them for parts
        for(Json::Value::iterator it = nodes.begin(); it != nodes.end(); it++)
        {
            loadPartsNodeSearch(*it);
        }
    }
}

// Recursive worker for the parts loader
void Model::loadPartsNodeSearch(const Json::Value &node)
{
    // Break out if this node is invalid
    if(node == Json::Value::null || !node.isObject())
    {
        return;
    }
    
    // Get the id for global inverse lookup
    std::string id = node["id"].asString();
    
    // If we find that we have a parts entry, explore it
    const Json::Value& parts = node["parts"];
    if(parts != Json::Value::null)
    {
        for(Json::Value::iterator it = parts.begin(); it != parts.end(); it++)
        {
            // Allocate a part to store data in
            Model::MeshPartRenderData *renderable = new Model::MeshPartRenderData();
            
            // Look up the mesh part this renderable uses
            std::string meshpartid = (*it)["meshpartid"].asString();
            for(std::vector<Mesh *>::iterator mIt = meshes.begin(); mIt != meshes.end(); mIt++)
            {
                // Iterate through mesh parts and put them in a map
                for(std::vector<Mesh::Part *>::iterator pIt = (*mIt)->Parts().begin(); pIt != (*mIt)->Parts().end(); pIt++)
                {
                    // If this is the part we are looking for
                    if((*pIt)->id == meshpartid)
                    {
                        // Store this mesh part
                        renderable->meshpart = *pIt;
                        
                        // get the fuck out (nested loop, can't break, need to force the iterators to the end)
                        mIt = meshes.end() - 1;
                        break;
                    }
                }
            }
            
            // Lookup the material this mesh uses
            std::string materialid = (*it)["materialid"].asString();
            renderable->material = materials[materialid];
            
            // Load the bones
            const Json::Value& bones = (*it)["bones"];
            if(bones != Json::Value::null)
            {
                for(Json::Value::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
                {
                    // Allocate a bone and deserialize
                    Model::Bone *bone = new Model::Bone(*bIt);
                    
                    // Store in the bones list
                    renderable->bones.push_back(bone);
                }
            }
            
            // Store this renderable data in our renderable list
            renderables.push_back(renderable);
        }
    }
    
    // Explore children if we have any
    const Json::Value& childs = node["children"];
    if(childs != Json::Value::null)
    {
        for(Json::Value::iterator it = childs.begin(); it != childs.end(); it++)
        {
            // Explore the children
            loadPartsNodeSearch(*it);
        }
    }
}

// Upload the data to the GPU
void Model::Upload()
{
    // If we are uploaded, get outta here
    if(uploaded)
    {
        return;
    }
    
    // Upload the mesh data
    for(std::vector<Mesh *>::iterator it = meshes.begin(); it != meshes.end(); it++)
    {
        // Create a vertex buffer for this mesh
        GLuint vertexBufferObject = 0;
        glGenBuffers(1, &vertexBufferObject);
        
        // Upload the data to the GPU
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, (*it)->Data().size() * sizeof(GLfloat), (*it)->Data().data(), GL_STATIC_DRAW);
        
        // Post this mesh to the buffers list
        buffers[(*it)] = vertexBufferObject;
    }
    
    // Upload the mesh part data
    for(std::vector<MeshPartRenderData *>::iterator it = renderables.begin(); it != renderables.end(); it++)
    {
#if !(defined __ANDROID__)
        // Create the vertex array object for this renderable
        glGenVertexArrays(1, &(*it)->attributes);
#endif
        // Create the vertex buffer for the indices
        glGenBuffers(1, &(*it)->indices);
        
        // Upload the index data to the gpu
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*it)->meshpart->indices.size() * sizeof(GLuint), (*it)->meshpart->indices.data(), GL_STATIC_DRAW);
    }
    
    // Upload material textures
    for(std::map<std::string, Material *>::iterator mIt = materials.begin(); mIt != materials.end(); mIt++)
    {
        // Iterate through this material's textures and upload them
        for(Material::const_texture_iterator it = mIt->second->texturesBegin(); it != mIt->second->texturesEnd(); it++)
        {
            // Upload the texture
            textureCache.GetTexture(it->second);
        }
    }
    
    // We are now uploaded
    uploaded = true;
}

// Update function
void Model::Update(double delta, double now)
{
    // If we need to upload the mesh data, do so now
    if(!uploaded)
    {
        Upload();
    }
    
    // Other shit
}

// Render method - forward the base skeleton to the model
void Model::Draw(GLMeshProgram *program)
{
    Draw(program, *skeleton);
}

// Render method - provide a skeleton to the model
void Model::Draw(GLMeshProgram *program, const Node& _skeleton)
{
    // If we are not uploaded, throw an exception
    if(!uploaded)
    {
        //throw new std::runtime_error("void Model::Draw(const GLMeshProgram *program) - Attempted to draw a model not uploaded to the GPU");
        return;
    }
    
    // Iterate through all the renderables
    for(std::vector<Model::MeshPartRenderData *>::iterator renderable = renderables.begin(); renderable != renderables.end(); renderable++)
    {
// Android does not support vertex array objects, therefore, we always have to push shader data for draw.  sadness
#if !(defined __ANDROID__)
        // Bind the vertex array object
        glBindVertexArray((*renderable)->attributes);
        
        // Update the program attributes if the shader has changed
        if(previousProgram != program)
        {
#endif
            // Bind the vertex buffer of the mesh
            glBindBuffer(GL_ARRAY_BUFFER, buffers[(*renderable)->meshpart->mesh]);
            
            // Iterate through the vertex attributes (dynamic shader parameters, as model vertex attributes vary)
            for(VertexAttributes::const_iterator attribute = (*renderable)->meshpart->mesh->Attributes().begin(); attribute != (*renderable)->meshpart->mesh->Attributes().end(); attribute++)
            {
                // Convert the attribute key into the shader attribute (in the future these should be equivalent - via layout=x)
                GLuint programAttribute = INT_MAX;
                switch (attribute->attribute)
                {
                    case VertexAttributes::kAttributeVertex:
                        programAttribute = program->AttributeVertex();
                        break;
                        
                    case VertexAttributes::kAttributeNormal:
                        programAttribute = program->AttributeNormal();
                        break;
                        
                    case VertexAttributes::kAttributeTextureCoordinate:
                        // Return the right texture coordinate mapping
                        programAttribute = program->AttributeTexture(attribute->index);
                        break;
                        
                    case VertexAttributes::kAttributeBoneWeight:
                        // Return the correct bone weight mapping
                        programAttribute = program->AttributeBoneWeight(attribute->index);
                        break;
                        
                    default:
                        // Skip unknown attributes
                        continue;
                        //throw new std::runtime_error("void Model::Draw(const GLMeshProgram *program) - Unrecognized shader attribute");
                        //break;
                }
#if !(defined __ANDROID__)
                // Enable the vertex array object entry for this attribute
                glEnableVertexAttribArray(programAttribute);
#endif
                // Update the vertex attribute pointer
                glVertexAttribPointer(programAttribute, VertexAttributes::AttributeSize(attribute->attribute), GL_FLOAT, GL_FALSE, (*renderable)->meshpart->mesh->Attributes().AttributeFrameSize() * sizeof(GLfloat), (GLvoid *)(attribute->offset * sizeof(GLfloat)));
            }
            
            // Bind the index buffer of the mesh part
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*renderable)->indices);
#if !(defined __ANDROID__)
        }
#endif
        // Bind the material's textures to some texturing units
        for(Material::const_texture_iterator tIt = (*renderable)->material->texturesBegin(); tIt != (*renderable)->material->texturesEnd(); tIt++)
        {
            // Set the shader texture sampler to this texture unit
            glUniform1i(program->UniformTexture(tIt->first), tIt->first);
            
            // Active the texture unit for this texture
            glActiveTexture(GL_TEXTURE0 + tIt->first);
            
            // Bind the texture for this texture unit
            textureCache.GetTexture(tIt->second)->Bind();
        }
        
        // Set the reflectivity (should pull this from the material right??)
        glm::vec2 specular = glm::vec2(1.0, 1.0);
        glUniform2fv(glGetUniformLocation(program->GetId(), "material_reflectivity"), 2, (const GLfloat *) &specular);
        
        // Set all the bones according to the skeleton
        int boneIdx = 0;
        for(std::vector<Model::Bone *>::iterator bone = (*renderable)->bones.begin(); bone != (*renderable)->bones.end(); bone++, boneIdx++)
        {
            // Get the node which cooresponds to our bone
            const Node *node = _skeleton.FindNode((*bone)->id);
            
            // Calculate the final bone transform
            glm::mat4 finalTransform = node->TransformMatrix() * (*bone)->inverseTransformMatrix;
            
            // Upload the bone to the shader
            glUniformMatrix4fv(program->UniformBones() + (boneIdx*4), 1, GL_FALSE, (const GLfloat *) &finalTransform);
        }
        
        // Finally, we can draw the god damn mesh
        glDrawElements(GL_TRIANGLES, (*renderable)->meshpart->indices.size(), GL_UNSIGNED_INT, NULL);
    }
    
    // Store the previous program
    previousProgram = program;
}

const Node* Model::Skeleton() const
{
    return skeleton;
}

const std::map<std::string, Animation *>& Model::Animations() const
{
    return animations;
}

// Standard constructor (initialize everything)
Model::Bone::Bone()
    : id(""), transform(Transform())
{
    
}

// Deserialization constructor
Model::Bone::Bone(const Json::Value& value)
    : Model::Bone::Bone()
{
    // We first need to validate that this a Json object
    if(!value.isObject())
    {
        throw std::runtime_error("Model::Bone::Bone(const Json::Value& value) - value must be a object");
    }
    
    // Load the name of the material
    id = value["node"].asString();
    
    // Load the transform
    transform = Transform(value);
    
    // Create an inverse transform matrix
    inverseTransformMatrix = glm::inverse(transform.TransformMatrix());
}

// Standard constructor
Model::MeshPartRenderData::MeshPartRenderData()
    : meshpart(NULL), material(NULL), bones(0, NULL), attributes(0), indices(0)
{
    
}

Model::MeshPartRenderData::~MeshPartRenderData()
{
    // Iterate through mesh parts and put them in a map
    for(std::vector<Model::Bone *>::iterator bIt = bones.begin(); bIt != bones.end(); bIt++)
    {
        delete *bIt;
    }
}
