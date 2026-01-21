#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/PrimitiveMesh2D.h"

namespace EisEngine {
    using namespace ecs;
    namespace components {
        /// \n This component represents an entity's shape in the game world.
        /// \n It is recommended to define a Renderer to provide a color to fragments.
        class Mesh2D : public Component {
        public:
            /// \n Creates a 2D Mesh component.
            /// @param _primitive - a primitive mesh containing vertex and edge data.
            explicit Mesh2D(Game &engine, guid_t owner, const PrimitiveMesh2D &_primitive);
            Mesh2D(const Mesh2D &other) = delete;
            Mesh2D(Mesh2D &&other) noexcept;

            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;

            /// \n Draws the mesh onto the screen once per frame.
            void draw();
            /// \n primitive mesh definition, stores vertex and edge data.
            const PrimitiveMesh2D primitive;
        private:
            /// \n Vertex Buffer Object -> contains vertex attribute and index data.
            unsigned int VBO = 0;
            /// \n Element Buffer Object -> stores index data to avoid reusing coordinates in triangles.
            unsigned int EBO = 0;
        };
    }
}
