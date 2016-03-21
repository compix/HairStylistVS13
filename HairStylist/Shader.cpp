#include "Shader.h"
#include <GL/glew.h>
#include <string>
#include "file.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include "Logger.h"

void Shader::shaderErrorCheck(GLuint shader, const std::string& shaderPath)
{
    GLint result = GL_FALSE;
    GLint infoLogLength;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> shaderLog(std::max(infoLogLength, 1));
    glGetShaderInfoLog(shader, infoLogLength, nullptr, &shaderLog[0]);

    if (result == GL_FALSE)
        std::cout << "Shader comilation failed for " << shaderPath << std::endl;

    fprintf(stdout, "%s\n", &shaderLog[0]);
}

void Shader::programErrorCheck(GLuint program, std::initializer_list<std::string> shaderPaths)
{
    GLint result = GL_FALSE;
    GLint infoLogLength;

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programLog(std::max(infoLogLength, 1));
    glGetProgramInfoLog(program, infoLogLength, nullptr, &programLog[0]);

    if (result == GL_FALSE)
    {
        std::cout << "Linking the program failed for ";
        for (auto& shaderPath : shaderPaths)
            std::cout << shaderPath << " ";
        std::cout << "\n";
    }

    fprintf(stdout, "%s\n", &programLog[0]);
}

void Shader::load(const std::string& vsPath, const std::string& fsPath)
{
    if (m_loadedProgram)
        glDeleteProgram(m_shaderProgram);

    GLuint vsID = load(GL_VERTEX_SHADER, vsPath);
    GLuint fsID = load(GL_FRAGMENT_SHADER, fsPath);

    GLuint program = glCreateProgram();

    glAttachShader(program, vsID);
    glAttachShader(program, fsID);

    glLinkProgram(program);

    programErrorCheck(program, { vsPath, fsPath });

    glDeleteShader(vsID);
    glDeleteShader(fsID);

    GL_ERROR_CHECK();

    m_shaderProgram = program;
    m_loadedProgram = true;
}

Shader::Shader(const std::string& vsPath, const std::string& fsPath)
{
    load(vsPath, fsPath);
}

Shader::~Shader()
{
    if (m_loadedProgram)
        glDeleteProgram(m_shaderProgram);
}

void Shader::load(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath)
{
    if (m_loadedProgram)
        glDeleteProgram(m_shaderProgram);

    GLuint vsID = load(GL_VERTEX_SHADER, vsPath);
    GLuint fsID = load(GL_FRAGMENT_SHADER, fsPath);
    GLuint gsID = load(GL_GEOMETRY_SHADER, gsPath);

    GLuint program = glCreateProgram();

    glAttachShader(program, vsID);
    glAttachShader(program, fsID);
    glAttachShader(program, gsID);

    glLinkProgram(program);

    programErrorCheck(program, { vsPath, fsPath, gsPath });

    glDeleteShader(vsID);
    glDeleteShader(fsID);
    glDeleteShader(gsID);

    GL_ERROR_CHECK();

    m_shaderProgram = program;
    m_loadedProgram = true;
}

void Shader::bindTexture2D(GLuint texId, const std::string& textureName, GLint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texId);
    glUniform1i(glGetUniformLocation(m_shaderProgram, textureName.c_str()), textureUnit);
}

void Shader::setDirLight(const DirectionalLight& dirLight, const glm::mat4& view)
{
    setVec3("u_dirLight.ambient", dirLight.ambient);
    setVec3("u_dirLight.diffuse", dirLight.diffuse);
    setVec3("u_dirLight.specular", dirLight.specular);
    setVec3("u_dirLight.viewDirection", glm::vec3(view * glm::vec4(dirLight.direction, 0.0f)));
}

void Shader::setMaterial(const Material& material)
{
    setVec3("u_material.diffuse", material.diffuse);
    setVec4("u_material.specular", material.specular);
}

GLuint Shader::load(GLenum shaderType, const std::string& shaderPath)
{
    GLuint id = glCreateShader(shaderType);

    std::string source = file::readAsString(shaderPath);
    char const* vsSource = source.c_str();
    glShaderSource(id, 1, &vsSource, nullptr);
    glCompileShader(id);

    shaderErrorCheck(id, shaderPath);

    return id;
}

void Shader::bind()
{
    glUseProgram(m_shaderProgram);
}

void Shader::setCamera(const glm::mat4& view, const glm::mat4& proj) const
{
    glUniformMatrix4fv(getLocation("u_view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(getLocation("u_proj"), 1, GL_FALSE, &proj[0][0]);
}

void Shader::setModel(const glm::mat4& modelMatrix, bool setInverseTranspose) const
{
    glUniformMatrix4fv(getLocation("u_model"), 1, GL_FALSE, &modelMatrix[0][0]);

    if (setInverseTranspose)
    {
        glm::mat4 modelIT = glm::transpose(glm::inverse(modelMatrix));
        glUniformMatrix4fv(getLocation("u_modelIT"), 1, GL_FALSE, &modelIT[0][0]);
    }
}

void Shader::setColor(float r, float g, float b, float a) const
{
    glUniform4f(getLocation("u_color"), r, g, b, a);
}

void Shader::setColor(glm::vec3 color) const
{
    setColor(color.r, color.g, color.b, 1.0f);
}

void Shader::setColor(float r, float g, float b) const
{
    setColor(r, g, b, 1.0f);
}

void Shader::setMVP(const glm::mat4& mvp) const
{
    glUniformMatrix4fv(getLocation("u_modelViewProj"), 1, GL_FALSE, &mvp[0][0]);
}

void Shader::setFloat(const char* uniformName, float v) const
{
    glUniform1f(getLocation(uniformName), v);
}

void Shader::setVec2(const char* uniformName, float v1, float v2) const
{
    glUniform2f(getLocation(uniformName), v1, v2);
}

void Shader::setVec3(const char* uniformName, float v1, float v2, float v3) const
{
    glUniform3f(getLocation(uniformName), v1, v2, v3);
}

void Shader::setVec4(const char* uniformName, float v1, float v2, float v3, float v4) const
{
    glUniform4f(getLocation(uniformName), v1, v2, v3, v4);
}

void Shader::setVec2(const char* uniformName, const glm::vec2& v) const
{
    glUniform2f(getLocation(uniformName), v.x, v.y);
}

void Shader::setVec3(const char* uniformName, const glm::vec3& v) const
{
    glUniform3f(getLocation(uniformName), v.x, v.y, v.z);
}

void Shader::setVec4(const char* uniformName, const glm::vec4& v) const
{
    glUniform4f(getLocation(uniformName), v.x, v.y, v.z, v.w);
}

void Shader::setMat4(const char* uniformName, const glm::mat4& m) const
{
    glUniformMatrix4fv(getLocation(uniformName), 1, GL_FALSE, &m[0][0]);
}

void Shader::setMat3(const char* uniformName, const glm::mat3& m) const
{
    glUniformMatrix3fv(getLocation(uniformName), 1, GL_FALSE, &m[0][0]);
}

void Shader::setMat2(const char* uniformName, const glm::mat2& m) const
{
    glUniformMatrix2fv(getLocation(uniformName), 1, GL_FALSE, &m[0][0]);
}
