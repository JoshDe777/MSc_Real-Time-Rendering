#pragma once

#include "engine/utilities/Vector2.h"
#include "engine/utilities/Vector3.h"
#include <array>

namespace EisEngine {
    namespace utilities {
        class Bounds3D {
        public:
            explicit Bounds3D(
                float xMin,
                float xMax,
                float yMin,
                float yMax,
                float zMin,
                float zMax
            );

            /// \n REQUIRED ORDER: xMin, xMax, yMin, yMax, zMin, zMax.
            explicit Bounds3D(std::array<float, 6> bounds);
            explicit Bounds3D(const Vector3& bounds);

            static Bounds3D Merge(Bounds3D& A, Bounds3D& B);
            static Bounds3D Expand(Bounds3D& A, const Vector3& B);
            Bounds3D Expand(const Vector3& B);
            Vector3 Centre() const;

            Vector2 xBounds;
            Vector2 yBounds;
            Vector2 zBounds;

            bool HitByRay(
                    const EisEngine::Vector3 &raySource,
                    const EisEngine::Vector3 &rayDir,
                    float& t_max
            ) const;

            [[nodiscard]] Vector3 GetClosestPointTo(const Vector3& pos) const;

            [[nodiscard]] float diagonalSize() const;
        };
    }
}
