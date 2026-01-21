#include "engine/components/meshes/Mesh2D.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
    // helper functions:

    // create and fill an openGL buffer object of the specified type.
    template<typename T>
    GLuint CreateBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    // mesh2D methods:

    Mesh2D::Mesh2D(Game &engine, guid_t owner, const PrimitiveMesh2D &_primitive):
            Component(engine, owner),
            primitive(_primitive),
            VBO(CreateBuffer(GL_ARRAY_BUFFER, _primitive.vertices)),
            EBO(CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, _primitive.indices)) { }

    Mesh2D::Mesh2D(EisEngine::components::Mesh2D &&other) noexcept  :
            Component(other),
            primitive(other.primitive)
    {
        owner = other.owner;
        std::swap(this->VBO, other.VBO);
        std::swap(this->EBO, other.EBO);
    }

    void Mesh2D::Invalidate() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        Component::Invalidate();
    }

    void Mesh2D::draw() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(GL_TRIANGLES, primitive.indexCount, GL_UNSIGNED_INT, nullptr);
    }
}