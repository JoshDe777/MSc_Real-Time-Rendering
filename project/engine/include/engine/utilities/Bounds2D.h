#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include "Math.h"

namespace EisEngine {

    /// \n Defines the boundaries of a 2D-rectangular object.
    struct Bounds2D {
        /// \n Creates a new bounds object.
        explicit Bounds2D(float topBound = 0.5f,
                          float rightBound = 0.5f,
                          float bottomBound = -0.5f,
                          float leftBound = -0.5f) :
                top(topBound),
                right(rightBound),
                bottom(bottomBound),
                left(leftBound) {
            ReevaluateSize();
        }

        /// \n the top boundary of the box shape (y-axis).
        float top;
        /// \n the right boundary of the box shape (x-axis).
        float right;
        /// \n the bottom boundary of the box shape (y-axis).
        float bottom;
        /// \n the left boundary of the box shape (x-axis).
        float left;

        /// \n the size of the box shape.
        Vector2 size;

        /// \n normalizes the bounds.
        Bounds2D& normalize(const Vector3& scale);

        /// \n classic 1x1 square bounds.
        static const Bounds2D Square1x1;

        Bounds2D& operator *= (const Vector3& v);

        operator std::string() const {
            return "Bounds: (top) " + std::to_string(top) +
            ", (right) " + std::to_string(right) +
            ", (bottom) " + std::to_string(bottom) +
            ", (left) " + std::to_string(left) +
            " -> size: " + (std::string) size;
        }

        /// \n Recalculates the size vectors for the bounds.
        void ReevaluateSize()
        { size = Vector2(Math::Dist(left, right), Math::Dist(top, bottom));}
        /// \n Determines whether a point is within the bounds or not.
        [[nodiscard]] bool Contains(const Vector3& v) const
        { return (v.x >= left) && (v.x <= right) && (v.y <= top) && (v.y >= bottom);}
    };
}
