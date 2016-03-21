#pragma once
#include <GL/glew.h>
#include <string>

class Texture
{
public:
    Texture() {}
    ~Texture();

    operator GLuint() const { return m_glId; }

    void load(const std::string& path);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
private:
    GLuint m_glId{ 0 };
    int m_width{ 0 };
    int m_height{ 0 };

    bool m_loaded{ false };
};
