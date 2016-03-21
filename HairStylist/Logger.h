#pragma once
#include <sstream>
#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

class Logger
{
public:
    Logger();

    static void log(const std::string&  msg);
    static std::ostream& stream() { return std::cout; }
    static std::ostream& errorStream() { return std::cerr; }
};

// Need to use macros to get the LOG_INFO_ATTACHMENT right
#if _MSC_VER == 1800
#define LOG_INFO_ATTACHMENT ":\nFUNCTION: " << __FUNCTION__ << "\nFILE: " << __FILE__ << "\nLINE: " << __LINE__
#else
#define LOG_INFO_ATTACHMENT ":\nFUNCTION: " << __func__ << "\nFILE: " << __FILE__ << "\nLINE: " << __LINE__
#endif

#define ERROR(M) do {Logger::errorStream() << M << LOG_INFO_ATTACHMENT << "\n\n";} while(0)
#define EXIT(M) do {Logger::errorStream() << M << LOG_INFO_ATTACHMENT << "\n\n"; exit(1);} while(0)
#define LOG(M) do {Logger::stream() << M << "\n";} while(0)

#define SDL_ERROR_CHECK() do            \
{                                       \
    const char* error = SDL_GetError(); \
    if (*error != '\0')                 \
        ERROR("SDL: " << error);        \
    SDL_ClearError();                   \
} while (0)

#define GL_ERROR_CHECK() do {                    \
    GLenum errorCode;                            \
    const GLubyte *errorString;                  \
    if ((errorCode = glGetError()) != GL_NO_ERROR) \
    {                                            \
        errorString = gluErrorString(errorCode); \
        ERROR("OpenGL Error " << errorCode <<    \
              ": " << errorString);              \
    }                                            \
} while (0)
