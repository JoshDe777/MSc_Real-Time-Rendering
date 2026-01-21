#pragma once

#include "engine/Utilities.h"

namespace EisEngine {
    class Game;
    namespace ecs { class Entity;}
    namespace components {
        class Transform;
        class Line;
    }

    // yes name is confusing/bad. Was initially created to debug physics and kept the name for lack of a better one...
    /// \n Draws a line between two given points on screen, in the given color.
    class DebugLine {
        using Entity = ecs::Entity;
        using Transform = components::Transform;
    public:
        /// \n Creates a new DebugLine object.
        /// @param startPoint - Vector3: The local position of where the line should start.
        /// @param endPoint - Vector3: The local position of where the line should end.
        /// @param color - Color: The line's color on screen.
        DebugLine(Game &engine, const Vector3& startPoint, const Vector3& endPoint, const Color &color);

        /// \n Sets new line positions.
        void UpdateLinePosition(const Vector3& startPoint, const Vector3& endPoint);
        /// \n Sets a new color for the line.
        void UpdateColor(const Color& color);

        /// \n The transform for the line.
        Transform *transform;
        /// \n A function called when an object is intentionally deleted.
        void Invalidate();
    private:
        /// \n The entity bundling all the lines together.
        Entity *entity;
        /// \n A pointer to the engine instance.
        Game* engine = nullptr;
    };
}
