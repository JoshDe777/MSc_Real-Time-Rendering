#include <algorithm>
#include "engine/utilities/rendering/PrimitiveMesh3D.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Math.h"
#include "engine/utilities/Debug.h"

namespace EisEngine::rendering {
#pragma region helpers
    // converts a std::vector of Vector3's to a std::vector of glm::vec3's.
    inline std::vector<glm::vec3> Vector3ToGlmVector(const std::vector<Vector3>& v){
        std::vector<glm::vec3> result = {};
        for(auto i : v)
            result.emplace_back((glm::vec3) i);
        return result;
    }

    inline std::vector<Vector3> GlmVectorToVector3(const std::vector<glm::vec3>& v){
        std::vector<Vector3> result = {};
        for(auto i : v)
            result.emplace_back(i);
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

#pragma endregion

#pragma region constructors & operators
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
        primitiveRoot = BuildBVHTree(4);
    }

    PrimitiveMesh3D::PrimitiveMesh3D(EisEngine::rendering::PrimitiveMesh3D &&other) noexcept :
        vertices(other.vertices),
        normals(other.normals),
        uvs(other.uvs),
        tangents(other.tangents),
        bitangents(other.bitangents),
        nVerts(other.nVerts),
        primitiveRoot(std::move(other.primitiveRoot)),
        PrimitiveMesh(GlmVectorToVector3(other.vertices), other.indices)
    { }
#pragma endregion

#pragma region getters
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
#pragma endregion

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
    const PrimitiveMesh3D PrimitiveMesh3D::skybox() { return PrimitiveMesh3D(
            skyboxVertices, skyboxIndices
            );}

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

    const PrimitiveMesh3D PrimitiveMesh3D::cube() { return PrimitiveMesh3D(
            cubeVertices, cubeIndices,
            &cubeNormals, &cubeUVs
    );}

#pragma region mesh traversal stuff

    bool PrimitiveMesh3D::RayIntersectsWithBounds(const EisEngine::Vector3 &raySource,
                                                  const EisEngine::Vector3 &rayDir,
                                                  const std::array<float, 6> &bounds,
                                                  float& t_max) const {
        auto inverseDir = Vector3(1/rayDir.x, 1/rayDir.y, 1/rayDir.z);

        auto temp1 = Vector3(bounds[0], bounds[2], bounds[4]) - raySource;
        auto tEntry = Vector3(
                temp1.x * inverseDir.x,
                temp1.y * inverseDir.y,
                temp1.z * inverseDir.z
        );

        auto temp2 = Vector3(bounds[1], bounds[3], bounds[5]) - raySource;
        auto tExit = Vector3(
                temp2.x * inverseDir.x,
                temp2.y * inverseDir.y,
                temp2.z * inverseDir.z
        );

        auto tLatestEntry = max(min(tEntry.x, tExit.x), max(min(tEntry.y, tExit.y), min(tEntry.z, tExit.z)));

        // early exit if there IS a hit but it's further than the closest registered hit (performance)
        if (tLatestEntry >= t_max)
            return false;

        auto tEarliestExit = min(max(tEntry.x, tExit.x), min(max(tEntry.y, tExit.y), max(tEntry.z, tExit.z)));

        // max(tLatestEntry, 0.0f) for culling of rays originating behind the mesh
        return tEarliestExit >= max(tLatestEntry, 0.0f);
    }

    BVHHitResult
    PrimitiveMesh3D::RayIntersectsWithTriangle(
            const EisEngine::Vector3 &raySource,
            const EisEngine::Vector3 &rayDir,
            const std::array<unsigned int, 3> &triangle
    ) const {
        Vector3 edge1 = Vector3(vertices[triangle[1]] - vertices[triangle[0]]);
        Vector3 edge2 = Vector3(vertices[triangle[2]] - vertices[triangle[0]]);

        // gets vector orthogonal to the plane formed by the ray and edge 2
        Vector3 ortho = Vector3::CrossProduct(rayDir, edge2);
        // gets angle between above and edge 1
        // -> if close to 0 means rayDir near parallel to plane edge1 edge2
        float alpha = Vector3::DotProduct(edge1, ortho);

        // no hit if near parallel
        if(abs(alpha) < Math::EPSILON)
            return BVHHitResult(false);

        float alphaInverse = 1.0f / alpha;
        Vector3 sourceToV0 = raySource - Vector3(vertices[triangle[0]]);

        float uCoord = alphaInverse * Vector3::DotProduct(sourceToV0, ortho);

        // if barycentric u not in range [0, 1] -> ray misses this triangle
        if (uCoord < 0 || uCoord > 1)
            return BVHHitResult(false);

        auto vCrossE1 = Vector3::CrossProduct(sourceToV0, edge1);
        float vCoord = alphaInverse * Vector3::DotProduct(rayDir, vCrossE1);

        // if barycentric v not in range [0, 1] -> ray misses this triangle
        if (vCoord < 0 || vCoord > 1)
            return BVHHitResult(false);

        float dist = alphaInverse * Vector3::DotProduct(edge2, vCrossE1);
        if (dist < Math::EPSILON)
            return BVHHitResult(false);

        return BVHHitResult(true, dist, Vector2(uCoord, vCoord), triangle);
    }

    BVHHitResult
    PrimitiveMesh3D::GetBVHHit(
            const Vector3& raySource,
            const Vector3& rayDir,
            BVHNode* node,
            float& t_max
    ) const {
        if (node == nullptr){
            DEBUG_ERROR("Primitive not equipped for BVH traversal!")
            return BVHHitResult(false);
        }

        auto hit = RayIntersectsWithBounds(raySource, rayDir, node->bounds, t_max);

        // early exit if misses the mesh's bounding box entirely
        if(!hit)
            return BVHHitResult(false);

        // init dummy no hit result struct in case no hits at all.
        BVHHitResult closestHit = BVHHitResult(false);

        // end recursion if leaf mesh subdivision
        if(node->isLeaf()){
            // calculate ray hit for every triangle in bounds and return closest hit.
            for (auto i : node->triangles) {
                hit = RayIntersectsWithTriangle(raySource, rayDir, i);
                if (hit.hit && hit.dist < t_max) {
                    closestHit = hit;
                    t_max = hit.dist;
                }
            }
            return closestHit;
        }

        auto lHit = GetBVHHit(raySource, rayDir, node->left.get(), t_max);
        if (lHit.hit) {
            closestHit = lHit;
            t_max = lHit.dist;
        }

        auto rHit = GetBVHHit(raySource, rayDir, node->right.get(), t_max);
        if (rHit.hit)
            closestHit = rHit;

        return closestHit;
    }

    std::unique_ptr<BVHNode> PrimitiveMesh3D::BuildNode(
            std::vector<std::array<unsigned int, 3>>& triangles,
            const int& start, const int& end, const int& maxTrianglesPerLeaf
    ){
        int middle = (start + end) / 2;

        // build a bounding box around all triangles in node.
        float minX = 10000000000000, minY = 10000000000000, minZ = 10000000000000;
        float maxX = -10000000000000, maxY = -10000000000000, maxZ = -10000000000000;
        for(auto i = start; i < end; i++){
            glm::vec3 vert0 = vertices[triangles[i][0]];
            glm::vec3 vert1 = vertices[triangles[i][1]];
            glm::vec3 vert2 = vertices[triangles[i][2]];

            minX = min(minX, min(vert0.x, min(vert1.x, vert2.x)));
            minY = min(minY, min(vert0.y, min(vert1.y, vert2.y)));
            minZ = min(minZ, min(vert0.z, min(vert1.z, vert2.z)));

            maxX = max(maxX, max(vert0.x, max(vert1.x, vert2.x)));
            maxY = max(maxY, max(vert0.y, max(vert1.y, vert2.y)));
            maxZ = max(maxZ, max(vert0.z, max(vert1.z, vert2.z)));
        }

        std::array<float, 6> bounds = {minX, maxX, minY, maxY, minZ, maxZ};
        auto node = std::make_unique<BVHNode>(bounds);

        if(end - start <= maxTrianglesPerLeaf){
            node->triangles = std::vector<std::array<unsigned int, 3>>(triangles.begin() + start, triangles.begin() + end);
            return node;
        }

        glm::vec3 box_size = {bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]};
        auto maxVal = max(box_size.x, max(box_size.y, box_size.z));

        int axis = maxVal == box_size.x ? 0 : maxVal == box_size.y ? 1 : 2;


        std::sort(triangles.begin() + start, triangles.begin() + end,
                  [axis, this](const std::array<unsigned int, 3>& a, const std::array<unsigned int, 3>& b) {
                      return centroid(a)[axis] < centroid(b)[axis];
                  });

        node->left = BuildNode(triangles, start, middle, maxTrianglesPerLeaf);
        node->right = BuildNode(triangles, middle, end, maxTrianglesPerLeaf);

        return node;
    }

    std::array<float, 3> PrimitiveMesh3D::centroid(const std::array<unsigned int, 3> &a) {
        // arithmetic average of all vertices in a triangle.
        auto centroid = 0.33333f * (vertices[a[0]] + vertices[a[1]] + vertices[a[2]]);
        return {centroid.x, centroid.y, centroid.z};
    }

    std::unique_ptr<BVHNode> PrimitiveMesh3D::BuildBVHTree(const int &maxTrianglesPerLeaf) {
        std::vector<std::array<unsigned int, 3>> triangles = {};
        for(int i = 0; i < indices.size(); i += 3)
            triangles.emplace_back(std::array<unsigned int, 3>{indices[i], indices[i+1], indices[i+2]});

        return BuildNode(triangles, 0, (int) triangles.size(), maxTrianglesPerLeaf);
    }
#pragma endregion
}