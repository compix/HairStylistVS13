#include "Window.h"
#include "Logger.h"
#include <GL/glew.h>

Window::Window(int width, int height, bool enableVsync)
    :m_window(nullptr), m_context(nullptr), m_width(width), m_height(height)
{
    if(!init(enableVsync))
        EXIT("Window initialization failed.");    
}

Window::~Window()
{
    if(m_context)
    {
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }

    if(m_window)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

void Window::flip()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::setVsync(bool enableVsync)
{
    if (enableVsync)
    {
        if (SDL_GL_SetSwapInterval(-1) != 0)
            SDL_GL_SetSwapInterval(1);
    }
    else
        SDL_GL_SetSwapInterval(0);
}

void Window::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    SDL_SetWindowSize(m_window, width, height);
}

void Window::showMessageBox(const std::string& title, const std::string& message, Uint32 flags)
{
    SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), nullptr);
}

void Window::showErrorBox(const std::string& title, const std::string& message)
{
    showMessageBox(title, message, SDL_MESSAGEBOX_ERROR);
}

bool Window::init(bool enableVsync)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_ERROR_CHECK();
        return false;
    }

    m_window = SDL_CreateWindow("Hello World!",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       m_width, m_height,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!m_window)
    {
        SDL_ERROR_CHECK();
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context)
    {
        ERROR("Failed to create OpenGL context. Make sure your GPU supports OpenGL 3.3");
        showErrorBox("Context Creation Error", "Failed to create OpenGL context. Make sure your GPU supports OpenGL 3.3");
        return false;
    }

    setVsync(enableVsync);
	glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (err != GLEW_OK)
    {
        const GLubyte *errString = gluErrorString(err);
        ERROR("Error " << err << " occured when initializing glew: \n" << errString);
        std::stringstream ss;
        ss << "Error " << err << " occured when initializing glew: \n" << errString;
        showErrorBox("GLEW Initialization Error", ss.str());
        return false;
    }

    // OpenGL 3.2+ might mistakenly report an invalid enum error. Ignore it:
    glGetError();

    return true;
}
