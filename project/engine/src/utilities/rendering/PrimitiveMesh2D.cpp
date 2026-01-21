#include "engine/utilities/rendering/PrimitiveMesh2D.h"
#include "engine/Utilities.h"

namespace EisEngine::rendering{
// helper functions:

    // converts a std::vector of Vector3's to a std::vector of glm::vec3's.
    inline std::vector<glm::vec3> Vector3ToGlmVector(const std::vector<Vector3>& v){
        std::vector<glm::vec3> result = {};
        for(auto i : v)
            result.emplace_back((glm::vec3) i);
        return result;
    }

// PrimitiveMesh2D members

    // primitive 1x1 square mesh
    const PrimitiveMesh2D PrimitiveMesh2D::Square = PrimitiveMesh2D(
            {
                    Vector3(-0.5f, 0.5f, 0),
                    Vector3(-0.5f, -0.5f, 0),
                    Vector3(0.5f, -0.5f, 0),
                    Vector3(0.5f, 0.5f, 0)
            },
            {
                    0, 3, 2,
                    0, 2, 1
            }
    );

    PrimitiveMesh2D::PrimitiveMesh2D(const vector<Vector3> &primitiveVertices,
                                     const vector<unsigned int> &shapeIndices) :
            vertices(Vector3ToGlmVector(primitiveVertices)),
            PrimitiveMesh(primitiveVertices, shapeIndices) { }

    std::vector<Vector3> PrimitiveMesh2D::GetVertices() const {
        std::vector<Vector3> result = {};
        result.reserve(vertices.size());
        for(auto i : vertices)
            result.emplace_back(i);
        return result;
    }
}
