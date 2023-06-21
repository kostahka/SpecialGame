#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif
#include <iostream>

namespace Kengine
{

void gl_get_error(int line, const char* file)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cerr << "Error at line " << line << ", at file " << file
                  << std::endl;
        switch (error)
        {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
                break;
#ifndef __ANDROID__
            case GL_STACK_UNDERFLOW:
                std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL_STACK_OVERFLOW" << std::endl;
                break;
#endif
        }
    }
};
}; // namespace Kengine