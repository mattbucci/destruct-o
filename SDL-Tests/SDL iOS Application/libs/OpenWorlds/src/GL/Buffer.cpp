//
//  Buffer.cpp
//  OpenWorlds Engine
//
//  Created by Nathaniel Lewis on 1/29/14.
//  Copyright (c) 2014 E1FTW Games. All rights reserved.
//

#include "Buffer.hpp"

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        /* Standard constructor */
        Buffer::Buffer()
        {
            // Generate an OpenGL buffer object
            glGenBuffers(1, &this->handle);
        }
        
        /* Bind a buffer */
        void Buffer::bind(GLenum target)
        {
            // Bind this buffer to an OpenGL buffer binding point
            glBindBuffer(target, this->handle);
        }
        
        /* Unbind a buffer */
        void Buffer::unbind(GLenum target)
        {
            // Unbind a buffer
            glBindBuffer(target, 0);
        }
    }
}