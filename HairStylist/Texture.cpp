#include "Texture.h"
#include <SOIL2.h>
#include "Logger.h"

Texture::~Texture()
{
    glDeleteTextures(1, &m_glId);
}

void Texture::load(const std::string& path)
{
    if (m_loaded)
        glDeleteTextures(1, &m_glId);

    int channels;
    auto imgData = SOIL_load_image(path.c_str(), &m_width, &m_height, &channels, SOIL_LOAD_AUTO);

    m_glId = SOIL_create_OGL_texture(
        imgData,
        &m_width, &m_height, channels,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

    SOIL_free_image_data(imgData);
    GL_ERROR_CHECK();

    m_loaded = true;
}
