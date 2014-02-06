//
//  Program.hpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Program is a class to wrap an
//  OpenGL shader program.  Multiple shader objects
//  are attach on create (vertex, fragment, etc.).
//

#ifndef __OpenWorlds_GL_Program__
#define __OpenWorlds_GL_Program__

/* Include the platform abstractor header for the GL layer */
#include "gl_includes.hpp"

/* OpenWorlds dependencies */
#include "Shader.hpp"
#include "Exception.hpp"

/* C++ STL dependencies */
#include <string>
#include <memory>

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Class to encapsulate an OpenGL shader program */
        class Program
        {
        private:
            /* OpenGL object handle */
            GLuint          handle;
            
            /* Potential OpenGL shader objects */
            std::shared_ptr<Shader> vertexShader;
            std::shared_ptr<Shader> fragmentShader;
            
#warning Add android detection, as it doesn't have geometry or tesselation
#ifndef __IPHONEOS__
            std::shared_ptr<Shader> geometryShader;
            std::shared_ptr<Shader> tesselationControlShader;
            std::shared_ptr<Shader> tesselationEvaluationShader;
#endif
            /* Link the shader program */
            void link();
            
        public:
            /* Create an OpenGL shader program from a vertex shader and fragment shader */
            Program(std::string vertexShaderFilename, std::string fragmentShaderFilename);
            
#warning Add android detection, as it doesn't have geometry or tesselation
#ifndef __IPHONEOS__
            /* Create an OpenGL shader program from a vertex shader, fragment shader, and geometry shader */
            Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename);
            
            /* Create an OpenGL shader program from a vertex shader, fragment shader, and tesselation shader */
            Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string tesselationControlShaderFilename, std::string tesselationEvaluationShaderFilename);
            
            /* Create an OpenGL shader program from a vertex shader, fragment shader, geometry shader, and tesselation shader */
            Program(std::string vertexShaderFilename, std::string fragmentShaderFilename, std::string geometryShaderFilename, std::string tesselationControlShaderFilename, std::string tesselationEvaluationShaderFilename);
#endif
            /* Copy another shader program */
            Program(const Program& program);
            
            /* Standard deconstructor */
            ~Program();
            
            /* Assign to other shader program */
            Program& operator=(const Program& program);
            
            /* Get the handle for the shader object */
            GLuint nativeHandle();
        };
    }
}

#endif /* defined(__OpenWorlds_GL_Program__) */

