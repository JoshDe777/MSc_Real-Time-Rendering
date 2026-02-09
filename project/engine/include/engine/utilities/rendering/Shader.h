#pragma once

#include <filesystem>
#include <OpenGL/OpenGlInclude.h>
#include "glm/glm.hpp"

#include "engine/utilities/rendering/Texture2D.h"
#include "engine/utilities/rendering/Cubemap.h"

namespace fs = std::filesystem;

namespace EisEngine{
    namespace systems{
        class Camera;
    }

    using namespace systems;

    enum UniformSamplerIndices{
        DIFFUSE = 0,
        NORMAL = 1,
        CUBEMAP = 2,
        DEPTH_BACK_FACE = 3,
        DEPTH_FRONT_FACE = 4
    };

    namespace rendering {
        /// \n Intermediary system from engine code to pixels on screen.
        class Shader {
        public:
            /// \n Creates a shader from the given shader programs.
            explicit Shader(const unsigned int& vertexShaderProgram, const unsigned int& fragmentShaderProgram);
            Shader(const Shader& other) = delete;
            Shader(Shader&& other) noexcept;

            /// \n Applies the shader to the rendering pipeline.
            void Apply(Camera* camera);
            /// \n Applies a texture to the rendering pipeline.
            void ApplyTexture2D(const Texture2D& texture, UniformSamplerIndices type) const;
            /// \n Applies a cubemap texture to the rendering pipeline.
            void ApplyCubemap(const Cubemap& cubemap) const;

            /// \n Sets a given uniform matrix in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the matrix whose values are to be set.
            /// @param mat4 - a 4x4 matrix representing the new desired value.
            void setMatrix(const std::string &uniformName, glm::mat4 mat4) const;

            /// \n Sets a given uniform matrix in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the matrix whose values are to be set.
            /// @param mat3 - a 4x4 matrix representing the new desired value.
            void setMatrix(const std::string &uniformName, glm::mat3 mat3) const;

            /// \n Sets a given uniform vector in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the vector whose values are to be set.
            /// @param vec4 - a 4D-vector representing the new desired value.
            void setVector(const std::string &uniformName, glm::vec4 vec4) const;

            /// \n Sets a given uniform vector in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the vector whose values are to be set.
            /// @param vec3 - a 3D-vector representing the new desired value.
            void setVector(const std::string &uniformName, glm::vec3 vec3) const;

            /// \n Sets a given uniform integer in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the integer whose values are to be set.
            /// @param val - an int to take on the value of the given parameter.
            void setInt(const std::string& uniformName, const int& val) const;

            /// \n Sets a given uniform float in the shader program to the specified value.
            /// @param uniformName - a string representing the name of the float whose values are to be set.
            /// @param val - a float to take on the value of the given parameter.
            void setFloat(const std::string& uniformName, const float& val) const;

            /// \n Multiplies the provided view-projection with the model matrix to give object position in camera space.
            /// @param modelMatrix: a 4x4 matrix representing object coordinates in world space.
            /// @param vpMatrix: a 4x4 matrix representing the screen projection from the camera's position.
            /// @return a 4x4 matrix representing object coordinates in camera space.
            [[nodiscard]] glm::mat4 CalculateMVPMatrix(const glm::mat4& modelMatrix) const { return vpMatrix * modelMatrix;}

            /// \n A function called when an object is intentionally deleted.
            void Invalidate() const;
            /// \n gets the shader program ID.
            unsigned int GetShaderID() const {return shaderProgram;}
        private:
            /// \n The OpenGL shader program.
            unsigned int shaderProgram = 0;
            /// \n This component's vertex shader.
            unsigned int vertexShader = 0;
            /// \n This component's fragment shader.
            unsigned int fragmentShader = 0;

            /// \n The last saved view-projection matrix.
            glm::mat4 vpMatrix = glm::mat4(1.0f);
        };
    }
}


