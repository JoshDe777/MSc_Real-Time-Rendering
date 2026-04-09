#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/PrimitiveMesh3D.h"

namespace EisEngine {
    using namespace ecs;
    namespace components {
        /// \n This component represents an entity's shape in the 3D world.\n
        class Mesh3D : public Component {
        public:
            explicit Mesh3D(Game& engine, guid_t owner, std::unique_ptr<PrimitiveMesh3D> _primitive);
            Mesh3D(const Mesh3D &other) = delete;
            Mesh3D(Mesh3D &&other) noexcept;

            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;

            /// \n Draws the mesh onto the screen once per frame.
            void draw(const unsigned int& shaderProgram);
            /// \n primitive mesh definition, stores vertex and edge data.
            std::unique_ptr<PrimitiveMesh3D> primitive;

            /// \n Finds the 3 vertices closest using BVH traversal to get the affected triangle
            Vector3 GetNormalAtRayIntersect(const Vector3& lightPos, const Vector3& lightDir) const;
        private:
            /// \n Vertex Buffer Object -> contains vertex attribute and index data.
            unsigned int VBO = 0;
            /// \n Element Buffer Object -> stores index data to avoid reusing coordinates in triangles.
            unsigned int EBO = 0;
        };

    }
}
