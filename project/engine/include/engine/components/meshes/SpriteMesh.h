#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/PrimitiveSpriteMesh.h"

namespace EisEngine {
    using ecs::Component;
    using rendering::PrimitiveSpriteMesh;

    namespace components {
        /// \n A mesh displaying a sprite.
        /// \n Requires a Renderer component for proper use, unlocking the use of materials and textures.
        class SpriteMesh : public Component {
        public:
            /// \n Creates a new sprite mesh.
            /// @param _primitive - PrimitiveMesh2D: The primitive shape of the mesh.
            explicit SpriteMesh(Game &engine, guid_t owner,
                                PrimitiveSpriteMesh _primitive = PrimitiveSpriteMesh::SquareSpriteMesh);
            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;

            /// \n the primitive mesh shape.
            const PrimitiveSpriteMesh primitive;
            /// \n A function called once every frame to display the mesh on screen.
            void draw();
        private:
            /// \n Vertex Buffer Object. Stores vertex data.
            unsigned int VBO;
            /// \n Element Buffer Object. Stores indices for triangle formation.
            unsigned int EBO;
        };
    }
}
