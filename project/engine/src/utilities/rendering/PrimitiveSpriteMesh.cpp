#include "engine/utilities/rendering/PrimitiveSpriteMesh.h"
#include "engine/utilities/Vector2.h"

namespace EisEngine::rendering {
// helper functions:

    // maps regular mesh vertices to texture positions, and compiles the information to a SpriteVertex.
    std::vector<SpriteVertex> RegularVerticesToSpriteVertices(const std::vector<Vector3>& vertices, Bounds2D& bounds){
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();

        for (const auto& vertex : vertices) {
            if (vertex.x < minX) minX = vertex.x;
            if (vertex.x > maxX) maxX = vertex.x;
            if (vertex.y < minY) minY = vertex.y;
            if (vertex.y > maxY) maxY = vertex.y;
        }

        bounds.top = maxY;
        bounds.right = maxX;
        bounds.bottom = minY;
        bounds.left = minX;
        bounds.ReevaluateSize();

        std::vector<SpriteVertex> spriteVertices;
        spriteVertices.reserve(vertices.size());

        for (const auto& vertex : vertices) {
            float normalizedX = (vertex.x - minX) / (maxX - minX);
            float normalizedY = 1 - ((vertex.y - minY) / (maxY - minY));

            Vector2 newTexCoord(normalizedX, normalizedY);
            spriteVertices.emplace_back(vertex, newTexCoord);
        }

        return spriteVertices;
    }

// PrimitiveSpriteMesh members:

    PrimitiveSpriteMesh::PrimitiveSpriteMesh(const std::vector<Vector3> &primitiveVertices,
                                             const std::vector<unsigned int> &shapeIndices) :
                                             bounds(),
                                             vertices(RegularVerticesToSpriteVertices(
                                                     primitiveVertices, bounds)),
                                             PrimitiveMesh(primitiveVertices, shapeIndices) { }

    const PrimitiveSpriteMesh PrimitiveSpriteMesh::SquareSpriteMesh = PrimitiveSpriteMesh({
        Vector3(-0.5f, 0.5f, 0),
        Vector3(-0.5f, -0.5f, 0),
        Vector3(0.5f, -0.5f, 0),
        Vector3(0.5f, 0.5f, 0)
    }, {
            0, 3, 2,
            0, 2, 1
    });

    std::vector<Vector3> PrimitiveSpriteMesh::GetVertices() const {
        std::vector<Vector3> result = {};
        result.reserve(vertices.size());
        for(auto i : vertices)
            result.emplace_back(i.modelPosition);
        return result;
    }
}