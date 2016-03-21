#pragma once
#include <GL/glew.h>
#include <string>

class Framebuffer
{
public:
    Framebuffer(GLsizei width, GLsizei height, bool hasRenderTexture);
    ~Framebuffer();

    /**
    * Starts drawing into this framebuffer.
    * All subsequent draw calls will affect this framebuffer.
    * Make sure to call end() when done drawing to this framebuffer.
    */
    void begin();

    /**
    * Ends drawing into this framebuffer.
    * All subsequent draw calls will affect the default framebuffer 0.
    */
    void end();

    GLuint getRenderTexture() const { return m_renderTexture; }

    void resizeRenderTexture(GLsizei width, GLsizei height);

    /**
    * Saves the render texture of this buffer to the given file in binary format.
    * If the filename does not exist then a new file will be created.
    */
    void saveRenderTexture(const std::string& filename);

    /**
    * Loads the render texture from the specified filename in binary format.
    * A new file will be created if it does not exist.
    */
    void loadRenderTexture(const std::string& filename);
private:
    GLenum m_format{ GL_RGB };
    GLsizei m_width;
    GLsizei m_height;
    GLuint m_fbo{0};
    GLuint m_renderTexture{0};
    bool m_hasRenderTexture{ false };
};

