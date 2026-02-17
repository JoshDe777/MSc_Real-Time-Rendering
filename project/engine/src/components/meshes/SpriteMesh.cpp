#include <utility>

#include "engine/components/meshes/SpriteMesh.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
    using EisEngine::rendering::SpriteVertex;

    // helper functions:

    // create and fill an openGL buffer object of the specified type.
    template<typename T>
    GLuint CreateBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T),
                     bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    // sprite mesh methods:

    SpriteMesh::SpriteMesh(Game &engine,
                           guid_t owner,
                           PrimitiveSpriteMesh _primitive) :
                           Component(engine, owner),
                           primitive(std::move(_primitive)),
                           VBO(CreateBuffer(GL_ARRAY_BUFFER, _primitive.vertices)),
                           EBO(CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, _primitive.indices)) { }

    void SpriteMesh::draw(const unsigned int& shader) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        auto vPos = glGetAttribLocation(shader, "aPos");
        if(vPos != -1){
            glEnableVertexAttribArray(vPos);
            glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(SpriteVertex),
                                  (void*)offsetof(SpriteVertex, modelPosition));
            DEBUG_OPENGL(entity()->name())
        }

        auto uvPos = glGetAttribLocation(shader, "texCoords");
        if(uvPos != -1){
            glEnableVertexAttribArray(uvPos);
            glVertexAttribPointer(uvPos, 2, GL_FLOAT,
                                  GL_FALSE, sizeof(SpriteVertex),
                                  (void*)offsetof(SpriteVertex, texturePosition));
            DEBUG_OPENGL(entity()->name())
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glDrawElements(GL_TRIANGLES, primitive.indexCount, GL_UNSIGNED_INT, nullptr);
        DEBUG_OPENGL(entity()->name())
    }

    void SpriteMesh::Invalidate() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        Component::Invalidate();
    }
}