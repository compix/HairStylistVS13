#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

using ShaderProgram = GLuint;

struct DirectionalLight
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
};

struct Material
{
    glm::vec3 diffuse;
    glm::vec4 specular; // w = specular power
};

class Shader
{
public:
    Shader() {}
    Shader(const std::string& vsPath, const std::string& fsPath);
    ~Shader();

    void load(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath);
    void load(const std::string& vsPath, const std::string& fsPath);
    ShaderProgram getProgram() const { return m_shaderProgram; }

    void bind();

    /**
    * Sets the camera view and proj matrices "u_view" and "u_proj".
    */
    void setCamera(const glm::mat4& view, const glm::mat4& proj) const;

    /**
    * Sets the model matrix "u_model".
    * If setInverseTranspose is true then "u_modelIT" 
    * will be set as the inverse transpose of the given model matrix.
    */
    void setModel(const glm::mat4& modelMatrix, bool setInverseTranspose = false) const;

    /**
    * Sets the color "u_color".
    */
    void setColor(float r, float g, float b, float a = 1.0f) const;

    /**
    * Sets the color "u_color". Alpha is set to 1.0f.
    */
    void setColor(glm::vec3 color) const;

    /**
    * Sets the color "u_color". Alpha is set to 1.0f.
    */
    void setColor(float r, float g, float b) const;

    /**
    * Sets the model view projection matrix "u_modelViewProj".
    */
    void setMVP(const glm::mat4& mvp) const;

    void setShaderProgram(ShaderProgram shaderProgram) { m_shaderProgram = shaderProgram; }

    bool hasSameProgram(ShaderProgram shaderProgram) const { return m_shaderProgram == shaderProgram; }

    void setFloat(const char* uniformName, float v) const;
    void setVec2(const char* uniformName, float v1, float v2) const;
    void setVec3(const char* uniformName, float v1, float v2, float v3) const;
    void setVec4(const char* uniformName, float v1, float v2, float v3, float v4) const;
    void setVec2(const char* uniformName, const glm::vec2& v) const;
    void setVec3(const char* uniformName, const glm::vec3& v) const;
    void setVec4(const char* uniformName, const glm::vec4& v) const;
    void setMat4(const char* uniformName, const glm::mat4& m) const;
    void setMat3(const char* uniformName, const glm::mat3& m) const;
    void setMat2(const char* uniformName, const glm::mat2& m) const;

    /**
    * textureName is the name of the texture in the shader.
    * Use 0 for the first sampler, 1 for the second ...
    */
    void bindTexture2D(GLuint texId, const std::string& textureName, GLint textureUnit = 0);

    /**
    * Sets the directional light "u_dirLight" with the following members:
    * vec3 ambient;
	* vec3 diffuse;
	* vec3 specular;
	* vec3 viewDirection;
    */
    void setDirLight(const DirectionalLight& dirLight, const glm::mat4& view);

    /**
    * Sets the material "u_material" with the following members:
    * vec3 diffuse;
    * vec4 specular;
    */
    void setMaterial(const Material& material);
private:
    GLuint load(GLenum shaderType, const std::string& shaderPath);
    void shaderErrorCheck(GLuint shader, const std::string& shaderPath);
    void programErrorCheck(GLuint program, std::initializer_list<std::string> shaderPaths);

    GLint getLocation(const char* uniformName) const
    {
        return glGetUniformLocation(m_shaderProgram, uniformName);
    }
private:
    ShaderProgram m_shaderProgram = 0;
    bool m_loadedProgram = false;
};

