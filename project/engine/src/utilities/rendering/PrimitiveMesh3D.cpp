#include "engine/utilities/rendering/PrimitiveMesh3D.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Math.h"
#include "engine/utilities/Debug.h"

namespace EisEngine::rendering {
    // converts a std::vector of Vector3's to a std::vector of glm::vec3's.
    inline std::vector<glm::vec3> Vector3ToGlmVector(const std::vector<Vector3>& v){
        std::vector<glm::vec3> result = {};
        for(auto i : v)
            result.emplace_back((glm::vec3) i);
        return result;
    }

    // converts a std::vector of Vector2's to a std::vector of glm::vec2's.
    inline std::vector<glm::vec2> Vector2ToGlmVector(const std::vector<Vector2>& v){
        std::vector<glm::vec2> result = {};
        for(auto i : v)
            result.emplace_back((glm::vec2) i);
        return result;
    }

    std::vector<glm::vec3> InitNormals(const std::vector<Vector3>* normals, const int& vCount){
        std::vector<glm::vec3> result = {};
        // if object has no normals, default to down.
        if(!normals){
            for(auto i = 0; i < vCount; i++)
                result.emplace_back(0, 0, -1);
        }
        else
            result = Vector3ToGlmVector(*normals);

        return result;
    }

    std::vector<glm::vec2> InitUVs(const std::vector<Vector2>* uvs, const int& vCount){
        std::vector<glm::vec2> result = {};
        // if object has no uvs, default to (0, 0)
        if(!uvs){
            for(auto i = 0; i < vCount; i++)
                result.emplace_back(0, 0);
        }
        else
            result = Vector2ToGlmVector(*uvs);

        return result;
    }

    PrimitiveMesh3D::PrimitiveMesh3D(
            const std::vector<Vector3> &shapeVertices,
            const std::vector<unsigned int> &shapeIndices,
            const std::vector<Vector3>* shapeNormals,
            const std::vector<Vector2>* shapeUVs,
            const std::vector<Vector3>* shapeTangents,
            const std::vector<Vector3>* shapeBitangents) :
            vertices(Vector3ToGlmVector(shapeVertices)),
            normals(InitNormals(shapeNormals, (int) shapeVertices.size())),
            uvs(InitUVs(shapeUVs, (int) shapeVertices.size())),
            nVerts(shapeVertices.size()),
            PrimitiveMesh(shapeVertices, shapeIndices) {
        CalculateTangentVecs();
        /*DEBUG_LOG("Init Primitive Mesh with " + std::to_string(nVerts) + " vertices, " +
            std::to_string(indices.size()) + " indices, " +
            std::to_string(normals.size()) + " normals, " +
            std::to_string(uvs.size()) + " UVs, " +
            std::to_string(tangents.size()) + " tangents, and " +
            std::to_string(bitangents.size()) + " bitangents."
        )*/
    }

    void PrimitiveMesh3D::CalculateTangentVecs() {
        std::vector<glm::vec3> tans = {nVerts, glm::vec3(0.0f)};
        std::vector<glm::vec3> bitans = {nVerts, glm::vec3(0.0f)};

        for(auto i = 0; i < indices.size(); i += 3){
            // prevent array index errors
            assert(i+2 < indices.size());
            // prevent reference errors looking for vertices that don't exist.
            assert(indices[i] < nVerts && indices[i+1] < nVerts && indices[i+2] < nVerts);

            glm::vec3 v0 = vertices[indices[i]];
            glm::vec3 v1 = vertices[indices[i+1]];
            glm::vec3 v2 = vertices[indices[i+2]];

            glm::vec2 uv0 = uvs[indices[i]];
            glm::vec2 uv1 = uvs[indices[i+1]];
            glm::vec2 uv2 = uvs[indices[i+2]];

            // determine local edges - vertex & uv (object space)
            auto e1 = v1 - v0;
            auto e2 = v2 - v0;

            auto uve1 = uv1 - uv0;
            auto uve2 = uv2 - uv0;

            // calculate det of UV matrix [uv1, uv2]
            auto det = uve1.x * uve2.y - uve1.y * uve2.x;
            // no valid operation here (matrix not invertible if det = 0)
            if(fabs(det) < 1e-6f) {
                /*DEBUG_INFO(
                    "Aborting tangent calculation for face:\n" +
                    (std::string) Vector3(v0) + ", " + (std::string) Vector2(uv0) + ",\n" +
                    (std::string) Vector3(v1) + ", " + (std::string) Vector2(uv1) + ",\n" +
                    (std::string) Vector3(v2) +  (std::string) Vector2(uv2) + ",\n" +
                    "(invalid determinant det=" + std::to_string(det) + ")"
                )*/
                continue;
            }

            auto detm1 = 1.0f / det;
            auto tan = detm1 * (uve2.y * e1 - uve1.y * e2);

            tans[indices[i]] += tan;
            tans[indices[i+1]] += tan;
            tans[indices[i+2]] += tan;
        }

        for(auto i = 0; i < nVerts; i++){
            auto& tan = tans[i];
            auto normal = normals[i];
            // no use normalizing empty vectors
            if(glm::length(tan) == 0){
                /*DEBUG_INFO("Aborting tangent calculation for vertex with normal " +
                    (std::string) Vector3(normal) + " (invalid tan.)")*/
               continue;
            }

            // Gram-Schmidt normalization for orthonormality to normal vector.
            tan = glm::normalize(tan - normal * glm::dot(normal, tan));
            // calculate bitangent as normal x tan
            bitans[i] = glm::cross(normal, tan);

            /*DEBUG_INFO("Created model space tangent vectors as:\n N=" + (std::string) Vector3(normal) +
            ",\n T=" + (std::string) Vector3(tan) +
            ",\n B=" + (std::string) Vector3(bitans[i]))*/
        }

        tangents = tans;
        bitangents = bitans;
    }

    std::vector<Vector3> PrimitiveMesh3D::GetVertices() const {
        std::vector<Vector3> result = {};
        result.reserve(vertices.size());
        for(auto i : vertices)
            result.emplace_back(i);
        return result;
    }

    std::vector<Vector3> PrimitiveMesh3D::GetNormals() const {
        std::vector<Vector3> result = {};
        result.reserve(normals.size());
        for(auto i : normals)
            result.emplace_back(i);
        return result;
    }

    std::vector<Vector2> PrimitiveMesh3D::GetUVs() const {
        std::vector<Vector2> result = {};
        result.reserve(uvs.size());
        for(auto i : uvs)
            result.emplace_back(i);
        return result;
    }

    std::vector<Vector3> PrimitiveMesh3D::GetTangents() const {
        std::vector<Vector3> result = {};
        result.reserve(tangents.size());
        for(auto i : tangents)
            result.emplace_back(i);
        return result;
    }

    std::vector<Vector3> PrimitiveMesh3D::GetBitangents() const {
        std::vector<Vector3> result = {};
        result.reserve(bitangents.size());
        for(auto i : bitangents)
            result.emplace_back(i);
        return result;
    }

#pragma region skybox definition
    const std::vector<Vector3> skyboxVertices = {
        Vector3(-1, -1, 1),       // 0
        Vector3(1, -1, 1),       // 1
        Vector3(1, -1, -1),       // 2
        Vector3(-1, -1, -1),       // 3
        Vector3(-1, 1, 1),       // 4
        Vector3(1, 1, 1),       // 5
        Vector3(1, 1, -1),       // 6
        Vector3(-1, 1, -1)        // 7
    };
    // indices clockwise
    const std::vector<unsigned int> skyboxIndices = {
        // right
        1, 2, 6,
        6, 5, 1,
        // left
        0, 4, 7,
        7, 3, 0,
        // top
        4, 5, 6,
        6, 7, 4,
        // bottom
        0, 3, 2,
        2, 1, 0,
        // back
        0, 1, 5,
        5, 4, 0,
        // front
        3, 7, 6,
        6, 2, 3
    };
#pragma endregion
    // no normals or UVs
    const PrimitiveMesh3D PrimitiveMesh3D::skybox = PrimitiveMesh3D(
            skyboxVertices, skyboxIndices
            );
}