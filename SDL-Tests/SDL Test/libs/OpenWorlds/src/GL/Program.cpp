//
//  Program.cpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Program is a class to wrap an
//  OpenGL shader program.  Multiple shader objects
//  are attach on create (vertex, fragment, etc.).
//

#include "Program.hpp"

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
        /* Create an OpenGL shader program from a vertex shader and fragment shader */
        Program::Program(std::string vertexShaderFilename, std::string fragmentShaderFilename)
        {
            // Error variable
            GLenum error;
            
            // Load the shader objects
            vertexShader   = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexShaderFilename);
            fragmentShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentShaderFilename);
            
            // Create an OpenGL program object
            handle = glCreateProgram();
            
            // Check if an error occurred
            if((error = glGetError()) != GL_NO_ERROR || handle <= 0)
            {
                // Throw a graphics exception
                throw Exception(error, "Failed to create shader program");
            }
            
            // Attach shader objects
            glAttachShader(handle, vertexShader->nativeHandle());
            glAttachShader(handle, fragmentShader->nativeHandle());
            
            // Link the program
            link();
        }
        
        /* Create an OpenGL shader program from a vertex shader, fragment shader, and geometry shader */
        Program::Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename)
        {
            // Error variable
            GLenum error;
            
            // Load the shader objects
            vertexShader   = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexShaderFilename);
            fragmentShader = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentShaderFilename);
            geometryShader = std::make_shared<Shader>(GL_GEOMETRY_SHADER, geometryShaderFilename);
            
            // Create an OpenGL program object
            handle = glCreateProgram();
            
            // Check if an error occurred
            if((error = glGetError()) != GL_NO_ERROR || handle <= 0)
            {
                // Throw a graphics exception
                throw Exception(error, "Failed to create shader program");
            }
            
            // Attach shader objects
            glAttachShader(handle, vertexShader->nativeHandle());
            glAttachShader(handle, fragmentShader->nativeHandle());
            glAttachShader(handle, geometryShader->nativeHandle());
            
            // Link the program
            link();
        }
        
        /* Create an OpenGL shader program from a vertex shader, fragment shader, and tesselation shader */
        Program::Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string tesselationControlShaderFilename, std::string tesselationEvaluationShaderFilename)
        {
            // Error variable
            GLenum error;
            
            // Load the shader objects
            vertexShader                = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexShaderFilename);
            fragmentShader              = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentShaderFilename);
            tesselationControlShader    = std::make_shared<Shader>(GL_TESS_CONTROL_SHADER, tesselationControlShaderFilename);
            tesselationEvaluationShader = std::make_shared<Shader>(GL_TESS_EVALUATION_SHADER, tesselationEvaluationShaderFilename);
            
            // Create an OpenGL program object
            handle = glCreateProgram();
            
            // Check if an error occurred
            if((error = glGetError()) != GL_NO_ERROR || handle <= 0)
            {
                // Throw a graphics exception
                throw Exception(error, "Failed to create shader program");
            }
            
            // Attach shader objects
            glAttachShader(handle, vertexShader->nativeHandle());
            glAttachShader(handle, fragmentShader->nativeHandle());
            glAttachShader(handle, tesselationControlShader->nativeHandle());
            glAttachShader(handle, tesselationEvaluationShader->nativeHandle());
            
            // Link the program
            link();
        }
        
        /* Create an OpenGL shader program from a vertex shader, fragment shader, geometry shader, and tesselation shader */
        Program::Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename, std::string tesselationControlShaderFilename, std::string tesselationEvaluationShaderFilename)
        {
            // Error variable
            GLenum error;
            
            // Load the shader objects
            vertexShader                = std::make_shared<Shader>(GL_VERTEX_SHADER, vertexShaderFilename);
            fragmentShader              = std::make_shared<Shader>(GL_FRAGMENT_SHADER, fragmentShaderFilename);
            geometryShader              = std::make_shared<Shader>(GL_GEOMETRY_SHADER, geometryShaderFilename);
            tesselationControlShader    = std::make_shared<Shader>(GL_TESS_CONTROL_SHADER, tesselationControlShaderFilename);
            tesselationEvaluationShader = std::make_shared<Shader>(GL_TESS_EVALUATION_SHADER, tesselationEvaluationShaderFilename);
            
            // Create an OpenGL program object
            handle = glCreateProgram();
            
            // Check if an error occurred
            if((error = glGetError()) != GL_NO_ERROR || handle <= 0)
            {
                // Throw a graphics exception
                throw Exception(error, "Failed to create shader program");
            }
            
            // Attach shader objects
            glAttachShader(handle, vertexShader->nativeHandle());
            glAttachShader(handle, fragmentShader->nativeHandle());
            glAttachShader(handle, geometryShader->nativeHandle());
            glAttachShader(handle, tesselationControlShader->nativeHandle());
            glAttachShader(handle, tesselationEvaluationShader->nativeHandle());
            
            // Link the program
            link();
        }
        
        /* Decontructor - destroy program */
        Program::~Program()
        {
            // If we have a valid shader program, destroy it
            if(handle > 0)
            {
                // Destroy the shader
                glDeleteProgram(handle);
                handle = -1;
                
                // Memory management will destroy the shaders
            }
        }
    
        /* Link the shader program */
        void Program::link()
        {
            // Attempt a link of the program
            glLinkProgram(handle);
            
            // Check if we linked successfully
            GLint linkSuccess;
            glGetProgramiv(handle, GL_LINK_STATUS, &linkSuccess);
            if(linkSuccess != GL_TRUE)
            {
                // Error hint string (use vector because writing directly to std::string buffers is undefined)
                std::vector<GLchar> errorLog;
                
                // Pull the error log size for the shader
                GLint length;
                glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
                errorLog.resize(length);
                
                // Pull the error log for the shader compilation
                glGetProgramInfoLog(handle, length, 0, errorLog.data());
                
                // Destroy the shader object
                glDeleteProgram(handle);
                
                // Throw graphics exception
                throw Exception(glGetError(), (std::string(errorLog.data())));
            }
        }
        
        // Copy constructor
        Program::Program(const Program& program)
        {
            // Do nothing, copying is not legal, could cause issues since we can't destroy the shader if someone copies us, could *possibly* implement
            // something like shared_ptr
        }
        
        // Assignment operator
        Program& Program::operator=(const Program &program)
        {
            // Do nothing, copying is not legal, could cause issues since we can't destroy the shader if someone copies us
            return *this;
        }
        
        // Get the native handle for the shader
        GLuint Program::nativeHandle()
        {
            return handle;
        }
    }
}