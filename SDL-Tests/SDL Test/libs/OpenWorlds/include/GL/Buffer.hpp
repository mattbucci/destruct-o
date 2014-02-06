//
//  Buffer.hpp
//  OpenWorlds Engine
//
//  Created by Nathaniel Lewis on 1/29/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Buffer is a class to wrap an
//  instance of an OpenGL buffer
//

#ifndef __OpenWorlds_GL_Buffer__
#define __OpenWorlds_GL_Buffer__

/* Include the platform abstractor header for the GL layer */
#include "gl_includes.hpp"

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Class to encapsulate an OpenGL buffer object */
        class Buffer
        {
        private:
            /* OpenGL object handle */
            GLuint          handle;
        
        public:
            /* Standard constructor */
            Buffer();
            
            /* Bind this buffer to a binding point*/
            void bind(GLenum target);
            
            /* Unbind a buffer from a binding point */
            static void unbind(GLenum target);
        };
    }
}

#endif /* defined(__OpenWorlds_GL_Buffer__) */
