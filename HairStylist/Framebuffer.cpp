#include "Framebuffer.h"
#include "Logger.h"
#include <fstream>
#include "file.h"

Framebuffer::Framebuffer(GLsizei width, GLsizei height, bool hasRenderTexture)
{
    m_width = width;
    m_height = height;
    m_hasRenderTexture = hasRenderTexture;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if (hasRenderTexture)
    {
        glGenTextures(1, &m_renderTexture);
        glBindTexture(GL_TEXTURE_2D, m_renderTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, m_format, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTexture, 0);
        // Specify buffer into which outputs from frag shader will be written
        GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_ERROR_CHECK();
}


Framebuffer::~Framebuffer()
{
    if (m_hasRenderTexture)
        glDeleteTextures(1, &m_renderTexture);

    glDeleteFramebuffers(1, &m_fbo);
}

void Framebuffer::begin()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glScissor(0, 0, m_width, m_height);
}

void Framebuffer::end()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resizeRenderTexture(GLsizei width, GLsizei height)
{
    if (!m_hasRenderTexture)
        return;

    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, m_format, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_width = width;
    m_height = height;
}

void Framebuffer::saveRenderTexture(const std::string& filename)
{
    if (!m_hasRenderTexture)
        return;

    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    size_t size = m_width * m_height * 3;
    char* pixels = new char[size];
    glGetTexImage(GL_TEXTURE_2D, 0, m_format, GL_UNSIGNED_BYTE, pixels);

    std::ofstream(filename, std::ios::binary).write(pixels, size);
    delete[] pixels;
}

void Framebuffer::loadRenderTexture(const std::string& filename)
{
    if (!m_hasRenderTexture)
        return;

    if (!file::exists(filename))
    {
        ERROR("Could not load " << filename << " because the file does not exist.");
        return;
    }

    size_t size = file::getSize(filename);
    char* pixels = new char[size];
    std::ifstream(filename, std::ios::binary).read(pixels, size);

    glBindTexture(GL_TEXTURE_2D, m_renderTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, pixels);
    delete[] pixels;
}
