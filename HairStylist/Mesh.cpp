#include "Mesh.h"
#include "Logger.h"
#include <fstream>
#include "convert.h"
#include "file.h"

void Mesh::Builder::reset()
{
    *this = Builder();
}

Mesh::Builder& Mesh::Builder::createVBO(size_t size, const void* data, GLenum usage)
{
    m_vboSize = size;
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    GL_ERROR_CHECK();
    return *this;
}

Mesh::Builder& Mesh::Builder::attribute(GLint size, GLenum type, GLuint attribDivisor, GLboolean normalized)
{
    // Cannot deduce offset if user provides custom offsets
    // so make sure it doesn't happen
    assert(m_deducedOffset);

    // Queue attributes to deduce stride
    m_attributes.push_back(Attr(size, type, attribDivisor, normalized));
    return *this;
}

Mesh::Builder& Mesh::Builder::attribute(GLint size, GLenum type, const void* offset, GLsizei stride, GLuint attribDivisor, GLboolean normalized)
{
    m_deducedOffset = false;
    m_attributes.push_back(Attr(size, type, attribDivisor, normalized, offset, stride));
    return *this;
}

void Mesh::Builder::finalize(Mesh& mesh)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    GL_ERROR_CHECK();

    size_t stride = 0;
    if (m_deducedOffset) // Compute stride
        for (Attr& attr : m_attributes)
            stride += attr.size * convert::sizeFromGLType(attr.type);

    GLuint idx = 0;
    size_t offset = 0;

    // Bind attributes
    for (Attr& attr : m_attributes)
    { 
        glEnableVertexAttribArray(idx);
        if (m_deducedOffset)
            glVertexAttribPointer(idx, attr.size, attr.type, attr.normalized, GLsizei(stride), reinterpret_cast<void*>(offset));
        else
        {
            stride = attr.stride;
            glVertexAttribPointer(idx, attr.size, attr.type, attr.normalized, GLsizei(stride), attr.offset);
        }
                
        if (attr.divisor != 0)
            glVertexAttribDivisor(idx, attr.divisor);

        // Compute offset
        offset += convert::sizeFromGLType(attr.type) * attr.size;
        ++idx;
        GL_ERROR_CHECK();
    }

    // Unbind all
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GL_ERROR_CHECK();

    mesh.m_vbo = m_vbo;
    mesh.m_ibo = m_ibo;
    mesh.m_vao = m_vao;
    mesh.m_indexCount = m_indexCount;
    mesh.m_vertexCount = m_vboSize / stride;
}

Mesh::~Mesh()
{
    if (m_vertexCount > 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);

        if (m_indexCount > 0)
            glDeleteBuffers(1, &m_ibo);
    }
}

void Mesh::load(const std::string& vbPath, const std::string& ibPath)
{
    // Do not call load multiple times
    assert(m_vertexCount == 0);

    std::vector<char> vertices;
    uint32_t floatCount;
    file::loadRawBuffer(vbPath, vertices, floatCount);

    std::vector<char> indices;
    uint32_t numIndices;
    file::loadRawBuffer(ibPath, indices, numIndices);

    Builder builder;
    builder.createVBO(floatCount * sizeof(float), &vertices[4])
        .attribute(3, GL_FLOAT)
        .attribute(2, GL_FLOAT)
        .attribute(3, GL_FLOAT)
        .attribute(3, GL_FLOAT)
        .attribute(3, GL_FLOAT)
        .createIBO<GLuint>(numIndices, &indices[4])
        .finalize(*this);

    m_renderMode = GL_TRIANGLES;
}

void Mesh::loadQuad()
{
    // Do not call load multiple times
    assert(m_vertexCount == 0);

    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.f));
    vertices.push_back(glm::vec3(0.5f, -0.5f, 0.f));
    vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.f));
    vertices.push_back(glm::vec3(0.5f, 0.5f, 0.f));

    Builder builder;
    builder.createVBO(vertices.size() * sizeof(glm::vec3), &vertices[0])
        .attribute(3, GL_FLOAT)
        .finalize(*this);

    m_renderMode = GL_TRIANGLE_STRIP;
}

void Mesh::bind()
{
    glBindVertexArray(m_vao);

    if (m_indexCount > 0)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
}

void Mesh::render()
{
    if (m_indexCount > 0)
        glDrawElements(m_renderMode, GLsizei(m_indexCount), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(m_renderMode, 0, GLsizei(m_vertexCount));
}

void Mesh::bindAndRender()
{
    bind();
    render();
}
