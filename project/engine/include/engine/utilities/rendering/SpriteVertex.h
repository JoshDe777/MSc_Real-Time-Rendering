#pragma once

#include <glm/glm.hpp>

#include "../Vector3.h"
#include "../Vector2.h"

namespace EisEngine::rendering {
    using EisEngine::Vector3;
    using EisEngine::Vector2;

    /// \n represents a vertex definition for sprite meshes, mapping a vertex coordinate to a texture position.
    struct SpriteVertex {
    public:
        /// \n Creates a new sprite vertex.
        SpriteVertex(const Vector3& vertexPos, const Vector2& texturePos) :
        modelPosition(vertexPos), texturePosition(texturePos) { }

        /// \n Vertex position in model space.
        glm::vec3 modelPosition;
        /// \n Vertex position in texture space.
        glm::vec2 texturePosition;
    };
}
