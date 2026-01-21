#include "glm/gtc/type_ptr.hpp"
#include "engine/utilities/rendering/Shader.h"
#include "engine/ResourceManager.h"
#include "engine/utilities/Color.h"
#include "engine/systems/Camera.h"

namespace EisEngine::rendering {
    Shader::Shader(const unsigned int &vertexShaderProgram, const unsigned int &fragmentShaderProgram) :
    vertexShader(vertexShaderProgram),
    fragmentShader(fragmentShaderProgram) {
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
    }

    Shader::Shader(EisEngine::rendering::Shader &&other) noexcept {
        std::swap(this->shaderProgram, other.shaderProgram);
        std::swap(this->vertexShader, other.vertexShader);
        std::swap(this->fragmentShader, other.fragmentShader);
    }

    void Shader::Invalidate() const {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::Apply(Camera* camera) {
        glUseProgram(shaderProgram);
        vpMatrix = camera->GetVPMatrix();
        setMatrix("mvp", vpMatrix);
        setVector("camPos", camera->transform->GetGlobalPosition());
    }

    void Shader::ApplyTexture(const Texture2D& texture) const {
        glActiveTexture(GL_TEXTURE0);
        texture.Bind();
        setInt("image", 0);
    }

    void Shader::setMatrix(const std::string &uniformName, glm::mat4 mat4) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat4));
    }
    void Shader::setMatrix(const std::string &uniformName, glm::mat3 mat3) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat3));
    }
    void Shader::setVector(const std::string &uniformName, glm::vec4 vec4) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform4fv(uniformLocation, 1, glm::value_ptr(vec4));
    }
    void Shader::setVector(const std::string &uniformName, glm::vec3 vec3) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform3fv(uniformLocation, 1, glm::value_ptr(vec3));
    }

    void Shader::setInt(const std::string &uniformName, const int &val) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform1i(uniformLocation, val);
    }

    void Shader::setFloat(const std::string &uniformName, const float &val) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform1f(uniformLocation, val);
    }

    const fs::path Shader::defaultVertexShaderPath = "shaders/vertexShader.vert";
    const fs::path Shader::defaultFragmentShaderPath = "shaders/fragmentShader.frag";
    const fs::path Shader::spriteVertexShaderPath = "shaders/spriteVertexShader.vert";
    const fs::path Shader::spriteFragmentShaderPath = "shaders/spriteFragmentShader.frag";
    const fs::path Shader::uiVertexShaderPath = "shaders/uiVertexShader.vert";
}
