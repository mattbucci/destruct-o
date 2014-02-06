//
//  Exception.cpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 2/4/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//
//  OpenWorlds::GL::Exception is a subclass of exception
//  that contains handles for the OpenGL subsystem
//

#include "Exception.hpp"

/* OpenWorlds Engine Namespace */
namespace OpenWorlds
{
    /* Graphics Layer Namespace */
    namespace GL
    {
        // Standard constructor - looks up error message for you from error code, hint can be provided by code
        Exception::Exception(GLenum errorCode, std::string errorHint)
            : glErrorCode(errorCode), glErrorHint(errorHint)
        {
            // TODO: Stringify the GL error code
        }
    }
}