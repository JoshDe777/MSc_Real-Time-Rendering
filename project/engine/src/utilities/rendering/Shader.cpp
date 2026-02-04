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

        glValidateProgram(shaderProgram);

        GLint status;
        glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &status);
        if (status != GL_TRUE)
        {
            char log[1024];
            glGetProgramInfoLog(shaderProgram, 1024, nullptr, log);
            std::cerr << log << std::endl;
        }

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
        glActiveTexture(GL_TEXTURE0 + UniformSamplerIndices::DIFFUSE);
        texture.Bind();
        setInt("image", UniformSamplerIndices::DIFFUSE);
    }

    void Shader::ApplyCubemap(const Cubemap& cubemap) const {
        glActiveTexture(GL_TEXTURE0 + UniformSamplerIndices::CUBEMAP);
        cubemap.Bind();
        setInt("cubemap", UniformSamplerIndices::CUBEMAP);
    }

    void Shader::setMatrix(const std::string &uniformName, glm::mat4 mat4) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat4));
    }
    void Shader::setMatrix(const std::string &uniformName, glm::mat3 mat3) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat3));
    }
    void Shader::setVector(const std::string &uniformName, glm::vec4 vec4) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniform4fv(uniformLocation, 1, glm::value_ptr(vec4));
    }
    void Shader::setVector(const std::string &uniformName, glm::vec3 vec3) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniform3fv(uniformLocation, 1, glm::value_ptr(vec3));
    }

    void Shader::setInt(const std::string &uniformName, const int &val) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniform1i(uniformLocation, val);
    }

    void Shader::setFloat(const std::string &uniformName, const float &val) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        if(uniformLocation != -1)
            glUniform1f(uniformLocation, val);
    }
}
