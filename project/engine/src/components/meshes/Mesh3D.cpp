#include "engine/components/meshes/Mesh3D.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
    void GLCheckError(const char* context, const std::string& entityName) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            DEBUG_ERROR("OpenGL Error [" + entityName + "::" + std::string(context) + "]: " + std::to_string(error))
        }
    }

    // create and fill an openGL buffer object of the specified type.
    template<typename T>
    GLuint CreateBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    GLuint CreateVBO(const PrimitiveMesh3D& primitive){
        // buffer initialization
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        // data preparation
        const auto& vertices = Vec3VectorToGlm(primitive.GetVertices());
        auto vsize = vertices.size() * sizeof(glm::vec3);
        const auto& normals = Vec3VectorToGlm(primitive.GetNormals());
        auto nsize = normals.size() * sizeof(glm::vec3);
        const auto& uvs = Vec2VectorToGlm(primitive.GetUVs());
        auto uvsize = uvs.size() * sizeof(glm::vec2);
        auto total_buffer_size = GLsizeiptr(vsize + nsize + uvsize);

        // buffer population
        glBufferData(GL_ARRAY_BUFFER, total_buffer_size, nullptr, GL_STATIC_DRAW);
        long long offset = 0;
        glBufferSubData(GL_ARRAY_BUFFER, offset, vsize, vertices.data());
        offset += vsize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, nsize, normals.data());
        offset += nsize;
        glBufferSubData(GL_ARRAY_BUFFER, offset, uvsize, uvs.data());

        return buffer;
    }

    Mesh3D::Mesh3D(EisEngine::Game &engine, EisEngine::ecs::guid_t owner, const PrimitiveMesh3D &_primitive) :
    Component(engine, owner),
    primitive(_primitive),
    VBO(CreateVBO(_primitive)),
    EBO(CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, _primitive.indices)) {

    }

    Mesh3D::Mesh3D(EisEngine::components::Mesh3D &&other)  noexcept  :
            Component(other),
            primitive(other.primitive)
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

    void Mesh3D::draw(const unsigned int& shaderProgram) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        GLCheckError("Mesh3D::DrawElements::VBO Binding", entity()->name());

        // draw vertices
        auto vpos = glGetAttribLocation(shaderProgram, "aPos");
        glEnableVertexAttribArray(vpos);
        glVertexAttribPointer(vpos, 3, GL_FLOAT, GL_FALSE,
                              0, nullptr);
        auto offset = primitive.GetVertexCount() * sizeof(glm::vec3);
        GLCheckError("Mesh3D::DrawElements::vertices", entity()->name());

        // add normals
        auto norm = glGetAttribLocation(shaderProgram, "normal");
        glEnableVertexAttribArray(norm);
        glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE,
                              0, (GLvoid*)offset);
        offset += primitive.GetNormalsCount() * sizeof(glm::vec3);
        GLCheckError("Mesh3D::DrawElements::normals", entity()->name());

        // add uvs
        auto uv = glGetAttribLocation(shaderProgram, "texCoords");
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE,
                              0, (GLvoid*)offset);
        GLCheckError("Mesh3D::DrawElements::uvs", entity()->name());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        GLCheckError("Mesh3D::DrawElements::indices", entity()->name());

        glDrawElements(GL_TRIANGLES, primitive.indexCount, GL_UNSIGNED_INT, nullptr);
        GLCheckError("Mesh3D::DrawElements", entity()->name());
    }
}