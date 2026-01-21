#include "engine/components/meshes/Line.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::components {
    // helper functions:

    // create and fill an openGL buffer object of the specified type.
    template<typename T>
    GLuint CreateBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType,
                     bufferData.size() * sizeof(glm::vec3),
                     bufferData.data(),
                     GL_STATIC_DRAW);
        return buffer;
    }

    // creates a standard vector of openGL vectors.
    std::vector<glm::vec3> VectorsToGlmVec3s(const Vector3 &start, const Vector3 &end){ return {start, end};}

    // line methods:
    Line::Line(Game &engine, guid_t owner, const Vector3& start, const Vector3& end):
            Component(engine, owner),
            startPoint(start),
            endPoint(end),
            lineCoordinates(VectorsToGlmVec3s(start, end)),
            VBO(CreateBuffer(GL_ARRAY_BUFFER, lineCoordinates)) { UpdateBufferData();}

    void Line::Invalidate() {
        glDeleteBuffers(1, &VBO);
        Component::Invalidate();
    }

    void Line::draw() const {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, 2);
    }

    void Line::SetStartPoint(const EisEngine::Vector3 &start) {
        startPoint = start;
        UpdateBufferData();
    }
    void Line::SetEndPoint(const EisEngine::Vector3 &end) {
        endPoint = end;
        UpdateBufferData();
    }
    void Line::SetPoints(const Vector3& start, const Vector3& end) {
        startPoint = start;
        endPoint = end;
        UpdateBufferData();
    }

    void Line::UpdateBufferData() {
        lineCoordinates = VectorsToGlmVec3s(startPoint, endPoint);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 2*sizeof(glm::vec3), lineCoordinates.data(), GL_STATIC_DRAW);
    }
}