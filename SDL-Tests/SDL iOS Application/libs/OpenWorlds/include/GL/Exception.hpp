//
//  Exception.hpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Exception is a subclass of exception
//  that contains handles for the OpenGL subsystem
//

#ifndef __OpenWorlds_GL_Exception__
#define __OpenWorlds_GL_Exception__

/* Include the platform abstractor header for the GL layer */
#include "gl_includes.hpp"

/* C++ STL dependencies */
#include <exception>
#include <string>
#include <sstream>

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Graphics Exception */
        class Exception : public std::exception
        {
            // OpenGL error code
            GLenum       glErrorCode;
            
            // OpenGL error hint
            std::string  glErrorHint;
            
            // OpenGL error message
            std::string  glErrorMessage;
            
            // Override the exception::what() method for our custom message
            virtual const char* what() const throw()
            {
                // Use a string string to build a response
                std::stringstream whatMessage;
                
                // Build the message
                whatMessage << "glError() = " << glErrorCode << ", " << glErrorMessage << " (hint: " << glErrorHint << ")" << std::endl;
                
                // Return the message
                return whatMessage.str().data();
            }
            
        public:
            // Standard constructor - looks up error message for you from error code, hint can be provided by code
            Exception(GLenum errorCode, std::string errorHint = "<TODO: provide hint from API>");
        };
    }
}

#endif /* defined(__OpenWorlds_GL_Exception__) */
