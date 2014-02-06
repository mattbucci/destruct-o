//
//  Shader.hpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Program is a class to wrap an
//  OpenGL shader program.  Multiple shader objects
//  are attach on create (vertex, fragment, etc.).
//

#ifndef __OpenWorlds_GL_Shader__
#define __OpenWorlds_GL_Shader__

/* Include the platform abstractor header for the GL layer */
#include "gl_includes.hpp"

/* OpenWorlds dependencies */
#include "Exception.hpp"

/* C++ STL dependencies */
#include <string>

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Class to encapsulate an OpenGL shader object */
        class Shader
        {
        private:
            /* OpenGL object handle */
            GLuint          handle;
            
        public:
            /* Load a shader from the disk and compile */
            Shader(GLenum shaderType, std::string shaderFile);

            /* Copy another shader program */
            Shader(const Shader& shader);
            
            /* Standard deconstructor */
            ~Shader();
            
            /* Assign to other shader object */
            Shader& operator=(const Shader& shader);
            
            /* Get the handle for the shader object */
            GLuint nativeHandle();
        };
    }
}

#endif /* defined(__OpenWorlds_GL_Shader__) */
