#include "engine/utilities/rendering/PrimitiveMesh3D.h"
#include "engine/utilities/Vector2.h"

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

    std::vector<glm::vec3> InitTans(const std::vector<Vector3>* tans, const int& vCount){
        std::vector<glm::vec3> result = {};
        // if object has no tangents, default to (1, 0, 0)
        if(!tans)
            for(auto i = 0; i < vCount; i++)
                result.emplace_back(1, 0, 0);
        else
            result = Vector3ToGlmVector(*tans);

        return result;
    }

    std::vector<glm::vec3> InitBitans(const std::vector<Vector3>* bitans, const int& vCount){
        std::vector<glm::vec3> result = {};
        // if object has no tangents, default to (0, 0, 1)
        if(!bitans)
            for(auto i = 0; i < vCount; i++)
                result.emplace_back(0, 0, 1);
        else
            result = Vector3ToGlmVector(*bitans);

        return result;
    }

    PrimitiveMesh3D::PrimitiveMesh3D(const std::vector<Vector3> &shapeVertices,
                                     const std::vector<unsigned int> &shapeIndices,
                                     const std::vector<Vector3>* shapeNormals,
                                     const std::vector<Vector2>* shapeUVs,
                                     const std::vector<Vector3>* shapeTangents,
                                     const std::vector<Vector3>* shapeBitangents) :
                                     vertices(Vector3ToGlmVector(shapeVertices)),
                                     normals(InitNormals(shapeNormals, (int) shapeVertices.size())),
                                     uvs(InitUVs(shapeUVs, (int) shapeVertices.size())),
                                     tangents(InitTans(shapeTangents, (int) shapeVertices.size())),
                                     bitangents(InitBitans(shapeBitangents, (int) shapeVertices.size())),
                                     nVerts(shapeVertices.size()),
                                     PrimitiveMesh(shapeVertices, shapeIndices) {}

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

#pragma region Cube Definition
    const std::vector<Vector3> cubeVertices = {
            // front
            Vector3(-0.5f, -0.5f, 0.5f),        //0
            Vector3(-0.5f, 0.5f, 0.5f),         //1
            Vector3(0.5f, 0.5f, 0.5f),          //2
            Vector3(0.5f, -0.5f, 0.5f),         //3
            // left
            Vector3(-0.5f, 0.5f, 0.5f),         //4
            Vector3(-0.5f, -0.5f, 0.5f),        //5
            Vector3(-0.5f, -0.5f, -0.5f),        //6
            Vector3(-0.5f, 0.5f, -0.5f),        //7
            // bottom
            Vector3(-0.5f, -0.5f, 0.5f),        //8
            Vector3(0.5f, -0.5f, 0.5f),         //9
            Vector3(0.5f, -0.5f, -0.5f),        //10
            Vector3(-0.5f, -0.5f, -0.5f),       //11
            // right
            Vector3(0.5f, -0.5f, 0.5f),         //12
            Vector3(0.5f, 0.5f, 0.5f),          //13
            Vector3(0.5f, 0.5f, -0.5f),         //14
            Vector3(0.5f, -0.5f, -0.5f),        //15
            // back
            Vector3(-0.5f, -0.5f, -0.5f),       //16
            Vector3(0.5f, -0.5f, -0.5f),        //17
            Vector3(0.5f, 0.5f, -0.5f),         //18
            Vector3(-0.5f, 0.5f, -0.5f),        //19
            // top
            Vector3(-0.5f, 0.5f, -0.5f),        //20
            Vector3(0.5f, 0.5f, -0.5f),         //21
            Vector3(0.5f, 0.5f, 0.5f),          //22
            Vector3(-0.5f, 0.5f, 0.5f),         //23
    };

    const std::vector<unsigned int> cubeIndices = {
            2, 1, 0,
            3, 2, 0,
            14, 13, 12,
            14, 12, 15,
            19, 18, 17,
            19, 17, 16,
            7, 6, 4,
            4, 6, 5,
            8, 9, 11,
            9, 11, 10,
            23, 22, 20,
            22, 21, 20
    };

    const Vector3 forward = Vector3(0, 0, 1);
    const Vector3 right = Vector3(1, 0, 0);
    const Vector3 up = Vector3(0, 1, 0);

    const std::vector<Vector3> cubeNormals = {
            forward,
            forward,
            forward,
            forward,
            -right,
            -right,
            -right,
            -right,
            -up,
            -up,
            -up,
            -up,
            right,
            right,
            right,
            right,
            -forward,
            -forward,
            -forward,
            -forward,
            up,
            up,
            up,
            up
    };

    const std::vector<Vector2> cubeUVs = {
            // face 1
            Vector2(0, 0),
            Vector2(0, 1),
            Vector2(1, 1),
            Vector2(1, 0),
            // face 2
            Vector2(1, 1),
            Vector2(1, 0),
            Vector2(0, 0),
            Vector2(0, 1),
            // face 3
            Vector2(1, 0),
            Vector2(0, 0),
            Vector2(0, 1),
            Vector2(1, 1),
            // face 4
            Vector2(0, 0),
            Vector2(0, 1),
            Vector2(1, 1),
            Vector2(1, 0),
            // face 5
            Vector2(1, 0),
            Vector2(0, 0),
            Vector2(0, 1),
            Vector2(1, 1),
            // face 6
            Vector2(0, 1),
            Vector2(1, 1),
            Vector2(1, 0),
            Vector2(0, 0),
    };
#pragma endregion

    const PrimitiveMesh3D PrimitiveMesh3D::cube = PrimitiveMesh3D(
            cubeVertices, cubeIndices,
            &cubeNormals, &cubeUVs
            );

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