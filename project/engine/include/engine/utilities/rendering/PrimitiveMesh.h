#pragma once

#include <vector>
#include "../Vector3.h"

namespace EisEngine::rendering {
    /// \n {Abstract class} Contains vertex and edge data for meshes.
    struct PrimitiveMesh {
    public:
        PrimitiveMesh(const std::vector<Vector3>& shapeVertices, const std::vector<unsigned int>& shapeIndices) :
                indices(shapeIndices),
                indexCount(static_cast<int>(shapeIndices.size())) { }

        /// \n The triangle indices for this mesh.
        const std::vector<unsigned int> indices;
        /// \n The amount of indices used.
        const int indexCount;

        /// \n Access the mesh's vertices.
        [[nodiscard]] virtual std::vector<Vector3> GetVertices() const = 0;
    };
}
