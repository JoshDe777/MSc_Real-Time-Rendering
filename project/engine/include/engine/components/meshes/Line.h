#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "OpenGL/OpenGlInclude.h"
#include "engine/ecs/Component.h"

namespace EisEngine::components{
    using Component = ecs::Component;

    /// \n A line between two points.
    /// \n It is recommended to add a Renderer component for colors.
    class Line : public Component {
    public:
        /// \n Creates a line between two points in the given color.
        /// @param start - Vector3: the start point of the line in world space.
        /// @param end - Vector3: the end point of the line in world space.
        Line(Game &engine, guid_t owner, const Vector3& start, const Vector3& end);

        /// \n A function called when the component is intentionally deleted.
        void Invalidate() override;

        /// \n returns a Vector3 representing this line's start point in world space.
        [[nodiscard]] const Vector3& GetStartPoint() const { return startPoint; }
        /// \n returns a vector3 representing this line's end point in world space.
        [[nodiscard]] const Vector3& GetEndPoint() const { return endPoint; }

        /// \n sets a line's start point.
        void SetStartPoint(const Vector3& start);
        /// \n sets a line's end point.
        void SetEndPoint(const Vector3& end);
        /// \n sets both end points for this line.
        /// @param start - Vector3: the line's new start point in world space.
        /// @param end - Vector3: the line's new end point in world space.
        void SetPoints(const Vector3& start, const Vector3& end);

        /// \n draws the line onto the screen once per frame.
        void draw(const unsigned int& shaderProgram) const;
    private:
        /// \n Updates the GL buffer data to current points values
        void UpdateBufferData();

        /// \n the point in world space at which the line starts.
        Vector3 startPoint;
        /// \n the point in world space at which the line ends.
        Vector3 endPoint;
        /// \n Vertex Buffer Object used for rendering.
        unsigned int VBO;
        /// \n OpenGL vector of 3D-coordinates used for the line end points.
        std::vector<glm::vec3> lineCoordinates;
    };
}

