#pragma once

#include "PrimitiveMesh.h"

namespace EisEngine{
    namespace components{ class Mesh2D;}
    namespace rendering {
        /// \n Contains vertex and edge data for primitive 2D meshes.
    struct PrimitiveMesh2D : public PrimitiveMesh {
        friend EisEngine::components::Mesh2D;
    public:
        /// \n Creates a primitive 2D mesh.
        /// @param primitiveVertices - std::vector&lt;Vector3>: describes the vertex positions.
        /// @param shapeIndices - std::vector&lt;unsigned int>: serves as an indicator for OpenGL triangles.
        explicit PrimitiveMesh2D(const std::vector<Vector3> &primitiveVertices,
                                 const std::vector<unsigned int> &shapeIndices);
        /// \n A standard 1x1 square.
        static const PrimitiveMesh2D Square;
        /// \n Access the mesh's vertices.
        [[nodiscard]] std::vector<Vector3> GetVertices() const override;
    private:
        /// \n The vertices forming the mesh.
        const std::vector<glm::vec3> vertices;
    };
}
}
