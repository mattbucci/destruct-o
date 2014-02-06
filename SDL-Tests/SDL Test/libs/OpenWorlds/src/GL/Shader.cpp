//
//  Shader.cpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Shader is a class to wrap the
//  compilation, error checking, and handling of
//  an OpenGL shader
//

#include "Shader.hpp"

/* C++ STL Dependencies */
#include <fstream>
#include <vector>

#include <iostream>

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Standard OpenWorlds::GL::Shader constructor - create a shader object of type and compile source from file to shader */
        Shader::Shader(GLenum shaderType, std::string shaderFilename)
        {
            // Variable to keep track of error
            GLenum error;
            
            // Open an input filestream for the provided shader file
            std::ifstream shaderFile(shaderFilename);
            
            // Get the contents of the file in a string (iterators are awesome, this gets optimized very well on unix systems)
            std::string shaderContents((std::istreambuf_iterator<char>(shaderFile)), (std::istreambuf_iterator<char>()));
            
            // Create an OpenGL shader object
            handle = glCreateShader(shaderType);
            
            // Check if an error occurred
            if((error = glGetError()) != GL_NO_ERROR || handle <= 0)
            {
                // Throw a graphics exception
                throw Exception(error, "Failed to create shader object");
            }
            
            // Shader source wants an array of strings, get a pointer variable
            const GLchar *shaderSources = static_cast<const GLchar *>(shaderContents.data());
            
            // Bind the sources to the shader
            glShaderSource(handle, 1, &shaderSources, NULL);
            
            // Compile the shader
            glCompileShader(handle);
            
            // Check if the shader compiled
            GLint compileSuccess;
            glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);
            if(compileSuccess != GL_TRUE)
            {
                // Error hint string (use vector because writing directly to std::string buffers is undefined)
                std::vector<GLchar> errorLog;

                // Pull the error log size for the shader
                GLint length;
                glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
                errorLog.resize(length);
                
                // Pull the error log for the shader compilation
                glGetShaderInfoLog(handle, length, 0, errorLog.data());
                
                // Destroy the shader object
                glDeleteShader(handle);
                
                // Throw graphics exception
                throw Exception(glGetError(), (std::string(errorLog.data())));
            }
        }
        
        // Copy constructor
        Shader::Shader(const Shader& shader)
        {
            // Do nothing, copying is not legal, could cause issues since we can't destroy the shader if someone copies us, could *possibly* implement
            // something like shared_ptr
        }
        
        // Standard deconstructor
        Shader::~Shader()
        {
            // If we have a valid shader program, destroy it
            if(handle > 0)
            {
                glDeleteShader(handle);
                handle = -1;
            }
        }
        
        // Assignment operator
        Shader& Shader::operator=(const OpenWorlds::GL::Shader &shader)
        {
            // Do nothing, copying is not legal, could cause issues since we can't destroy the shader if someone copies us
            return *this;
        }
        
        // Get the native handle for the shader
        GLuint Shader::nativeHandle()
        {
            return handle;
        }
    }
}