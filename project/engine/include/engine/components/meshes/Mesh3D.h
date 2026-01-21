#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/PrimitiveMesh3D.h"

namespace EisEngine {
    using namespace ecs;
    namespace components {
        /// \n This component represents an entity's shape in the 3D world.\n
        class Mesh3D : public Component {
        public:
            explicit Mesh3D(Game& engine, guid_t owner, const PrimitiveMesh3D& _primitive);
            Mesh3D(const Mesh3D &other) = delete;
            Mesh3D(Mesh3D &&other) noexcept;

            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;

            /// \n Draws the mesh onto the screen once per frame.
            void draw(const unsigned int& shaderProgram);
            /// \n primitive mesh definition, stores vertex and edge data.
            const PrimitiveMesh3D primitive;
        private:
            /// \n Vertex Buffer Object -> contains vertex attribute and index data.
            unsigned int VBO = 0;
            /// \n Element Buffer Object -> stores index data to avoid reusing coordinates in triangles.
            unsigned int EBO = 0;
        };

    }
}
