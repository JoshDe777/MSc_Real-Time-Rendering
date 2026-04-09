#include "engine/components/meshes/Mesh3D.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
#pragma region buffers & mesh lifetime
    // create and fill an openGL buffer object of the specified type.
    template<typename T>
    GLuint CreateBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    GLuint CreateVBO(PrimitiveMesh3D* primitive){
        // buffer initialization
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        // data preparation
        const auto& vertices = Vec3VectorToGlm(primitive->GetVertices());
        auto vsize = vertices.size() * sizeof(glm::vec3);
        const auto& normals = Vec3VectorToGlm(primitive->GetNormals());
        auto nsize = normals.size() * sizeof(glm::vec3);
        const auto& uvs = Vec2VectorToGlm(primitive->GetUVs());
        auto uvsize = uvs.size() * sizeof(glm::vec2);
        const auto& tans = Vec3VectorToGlm(primitive->GetTangents());
        auto tansize = tans.size() * sizeof(glm::vec3);
        const auto& bitans = Vec3VectorToGlm(primitive->GetBitangents());
        auto bitansize = bitans.size() * sizeof(glm::vec3);
        auto total_buffer_size = GLsizeiptr(vsize + nsize + uvsize + tansize + bitansize);

        // buffer population
        glBufferData(GL_ARRAY_BUFFER, total_buffer_size, nullptr, GL_STATIC_DRAW);
        long long offset = 0;
        glBufferSubData(GL_ARRAY_BUFFER, offset, vsize, vertices.data());
        DEBUG_OPENGL("Vertices")
        offset += vsize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, nsize, normals.data());
        DEBUG_OPENGL("Normals")
        offset += nsize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, uvsize, uvs.data());
        DEBUG_OPENGL("UVs")
        offset += uvsize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, tansize, tans.data());
        DEBUG_OPENGL("Tangents")
        offset += tansize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, bitansize, bitans.data());
        DEBUG_OPENGL("Bitangents")

        return buffer;
    }

    Mesh3D::Mesh3D(EisEngine::Game &engine, EisEngine::ecs::guid_t owner, std::unique_ptr<PrimitiveMesh3D> _primitive) :
    Component(engine, owner),
    primitive(std::move(_primitive)),
    VBO(CreateVBO(_primitive.get())),
    EBO(CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, _primitive->indices)) {

    }

    Mesh3D::Mesh3D(EisEngine::components::Mesh3D &&other)  noexcept  :
            Component(other),
            primitive(std::move(other.primitive))
    {
        owner = other.owner;
        std::swap(this->VBO, other.VBO);
        std::swap(this->EBO, other.EBO);
    }

    void Mesh3D::Invalidate() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        Component::Invalidate();
    }
#pragma endregion

    void Mesh3D::draw(const unsigned int& shaderProgram) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        DEBUG_OPENGL(entity()->name())
        unsigned long long offset = 0;
        auto nVerts = primitive->GetVertexCount();

        // draw vertices
        auto vpos = glGetAttribLocation(shaderProgram, "aPos");
        if(vpos != -1){
            glEnableVertexAttribArray(vpos);
            glVertexAttribPointer(vpos, 3, GL_FLOAT, GL_FALSE,
                                  0, nullptr);
            DEBUG_OPENGL(entity()->name())
        }
        offset = nVerts * sizeof(glm::vec3);

        // add normals
        auto norm = glGetAttribLocation(shaderProgram, "normal");
        if(norm != -1){
            glEnableVertexAttribArray(norm);
            glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE,
                                  0, (GLvoid*)offset);
            DEBUG_OPENGL(entity()->name())
        }
        offset += nVerts * sizeof(glm::vec3);

        // add uvs
        auto uv = glGetAttribLocation(shaderProgram, "texCoords");
        if(uv != -1){
            glEnableVertexAttribArray(uv);
            glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE,
                                  0, (GLvoid*)offset);
            DEBUG_OPENGL(entity()->name())
        }
        offset += nVerts * sizeof(glm::vec2);

        // add tangents
        auto tan = glGetAttribLocation(shaderProgram, "tan");
        if(tan != -1){
            glEnableVertexAttribArray(tan);
            glVertexAttribPointer(tan, 3, GL_FLOAT, GL_FALSE,
                                  0, (GLvoid*)offset);
            DEBUG_OPENGL(entity()->name())
        }
        offset += nVerts * sizeof(glm::vec3);

        // add bitangents
        auto bitan = glGetAttribLocation(shaderProgram, "bitan");
        if(bitan != -1){
            glEnableVertexAttribArray(bitan);
            glVertexAttribPointer(bitan, 2, GL_FLOAT, GL_FALSE,
                                  0, (GLvoid*)offset);
            DEBUG_OPENGL(entity()->name())
        }
        //offset += nVerts * sizeof(glm::vec3);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        DEBUG_OPENGL(entity()->name())

        glDrawElements(GL_TRIANGLES, primitive->indexCount, GL_UNSIGNED_INT, nullptr);
        DEBUG_OPENGL(entity()->name())
    }

    Vector3 Mesh3D::GetNormalAtRayIntersect(
            const EisEngine::Vector3 &lightPos,
            const EisEngine::Vector3 &lightDir
    ) const {
        // cast ray [delegated to primitive since direct access to mesh data]
        // returns a hit result struct -> hit: bool, dist = ray distance, uv barycentric coordinates for interpolation,
        // and the indices of the vertices shaping the closest intersecting triangle.

        // the raycasting logic was in great part ideated in pseudocode by Anthropic's Claude AI,
        // implemented in C++ by myself from the pseudocode basis.
        auto t_max = 10000000000.0f;
        auto hit = primitive->GetBVHHit(
                lightPos,
                lightDir,
                primitive->primitiveRoot.get(),
                t_max
        );

        if(!hit.hit) {
            auto NaN = std::numeric_limits<float>::quiet_NaN();
            return Vector3(NaN, NaN, NaN);
        }

        // barycentric coords = vertex weights in interpolation; uv, missing w
        // need to sum up to 1 so w = 1-u-v
        auto w = 1.0f - hit.barycentricCoords.x - hit.barycentricCoords.y;

        auto allNormals = primitive->GetNormals();
        auto n0 = allNormals[hit.triangle[0]];
        auto n1 = allNormals[hit.triangle[1]];
        auto n2 = allNormals[hit.triangle[2]];

        return Vector3(n0 * w + n1 * hit.barycentricCoords.x + n2 * hit.barycentricCoords.y).normalized();
    }
}