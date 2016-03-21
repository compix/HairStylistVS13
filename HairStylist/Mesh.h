#pragma once
#include <GL/glew.h>
#include <string>
#include "Logger.h"
#include <vector>

class Mesh
{
public:
    class Builder
    {
        /**
        * Used to queue attributes to deduce offset and stride.
        */
        struct Attr
        {
            Attr() :offset(nullptr) {}
            Attr(GLint size, GLenum type, GLuint divisor, GLboolean normalized)
                :size(size), type(type), divisor(divisor), normalized(normalized), offset(nullptr) {}
            Attr(GLint size, GLenum type, GLuint divisor, GLboolean normalized, const void* offset, size_t stride)
                :size(size), type(type), divisor(divisor), normalized(normalized), offset(offset), stride(stride) {}

            GLint size{ 0 };
            GLenum type{ 0 };
            GLuint divisor{ 0 };
            GLboolean normalized{ false };
            const void* offset;
            size_t stride{ 0 };
        };

    public:
        void reset();

        /**
        * Creates a vertex buffer object with the given size and a pointer to the data.
        * Usage is defined to be GL_STATIC_DRAW per default.
        */
        Builder& createVBO(size_t size, const void* data, GLenum usage = GL_STATIC_DRAW);

        /**
        * Creates an index buffer object with the given index count and a pointer to the data.
        * Specify the index type as a template paramter - e.g. createIBO<GLuint>()
        * Usage is defined to be GL_STATIC_DRAW per default.
        */
        template<class TIndexType>
        Builder& createIBO(size_t indexCount, const void* data, GLenum usage = GL_STATIC_DRAW);

        /**
        * Specify the next vertex attribute - offsets and stride will be deduced when finalize() is called.
        * Attributes in the vertex buffer object are expected to be interleaved.
        * Order of attribute() calls should correspond to the order of attributes in the interleaved buffer object.
        */
        Builder& attribute(GLint size, GLenum type, GLuint attribDivisor = 0, GLboolean normalized = false);

        /**
        * Specify the next vertex attribute with offset and stride.
        * Order of attribute() calls should correspond to the order of attributes in the buffer object.
        */
        Builder& attribute(GLint size, GLenum type, const void* offset, GLsizei stride, GLuint attribDivisor = 0, GLboolean normalized = false);

        /**
        * Creates a vertex array object and binds attributes.
        */
        void finalize(Mesh& mesh);
        
    private:
        GLuint m_vbo{ 0 };
        GLuint m_ibo{ 0 };
        GLuint m_vao{ 0 };

        size_t m_indexCount{ 0 };
        size_t m_vertexCount{ 0 };
        bool m_deducedOffset{ true };
        size_t m_vboSize;
        std::vector<Attr> m_attributes;
    };

    friend Builder;

    Mesh() {}
    ~Mesh();

    /**
    * Loads a mesh with the specifed vertex and index buffer paths.
    * The files are expected to be in little-endian binary format.
    * In both cases the first value in the file is a uint32_t (first 4 bytes)
    * indicating the number of following values of type float for the vertex buffer
    * and uint32_t for the index buffer.
    * The vertex buffer is expected to have interleaved vertex attributes with the following structure:
    * - Position:            3 floats
    * - Texture Coordinates: 2 floats
    * - Normal:              3 floats
    * - Tangent:             3 floats
    * - Bitangent:           3 floats
    */
    void load(const std::string& vbPath, const std::string& ibPath);

    /**
    * Loads a quad with a position attribute.
    * Texture coordinates are deduced from the position.
    * This quad has 4 vertices and is rendered with GL_TRIANGLE_STRIP thus 
    * has inconsistent triangle winding order -> Do not cull faces.
    */
    void loadQuad();

    void bind();
    void render();
    void bindAndRender();

private:
    GLuint m_vbo{ 0 };
    GLuint m_ibo{ 0 };
    GLuint m_vao{ 0 };

    size_t m_indexCount{ 0 };
    size_t m_vertexCount{ 0 };
    GLenum m_renderMode{ 0 };
};

template <class TIndexType>
Mesh::Builder& Mesh::Builder::createIBO(size_t indexCount, const void* data, GLenum usage)
{
    m_indexCount = indexCount;
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(TIndexType), data, usage);

    GL_ERROR_CHECK();
    return *this;
}
