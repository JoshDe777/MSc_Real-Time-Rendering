#pragma once

#include "PrimitiveMesh.h"

namespace EisEngine {
    namespace components{ class Mesh3D;}
    namespace rendering {

    struct PrimitiveMesh3D : public PrimitiveMesh {
        friend EisEngine::components::Mesh3D;
    public:
        explicit PrimitiveMesh3D(const std::vector<Vector3> &shapeVertices,
                                 const std::vector<unsigned int> &shapeIndices,
                                 const std::vector<Vector3>* shapeNormals = nullptr,
                                 const std::vector<Vector2>* shapeUVs = nullptr);
        /// \n A standard 1x1x1 cube.
        static const PrimitiveMesh3D cube;

        // \n Access a few counts
        [[nodiscard]] unsigned int GetVertexCount() const { return nVerts;}
        [[nodiscard]] unsigned int GetNormalsCount() const { return nNormals;}
        [[nodiscard]] unsigned int GetUVCount() const { return nUVs;}

        /// \n Access the mesh's vertices.
        [[nodiscard]] std::vector<Vector3> GetVertices() const override;
        [[nodiscard]] std::vector<Vector3> GetNormals() const;
        [[nodiscard]] std::vector<Vector2> GetUVs() const;
    private:
        const std::vector<glm::vec3> vertices;
        const std::vector<glm::vec3> normals;
        const std::vector<glm::vec2> uvs;
        const unsigned int nVerts;
        const unsigned int nNormals;
        const unsigned int nUVs;
    };

    } // rendering
} // EisEngine
