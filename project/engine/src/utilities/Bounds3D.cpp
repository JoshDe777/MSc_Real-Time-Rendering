#include "engine/utilities/Bounds3D.h"

namespace EisEngine::utilities {
    Bounds3D::Bounds3D(
        float xMin,
        float xMax,
        float yMin,
        float yMax,
        float zMin,
        float zMax):
    xBounds(xMin, xMax),
    yBounds(yMin, yMax),
    zBounds(zMin, zMax) { }

    Bounds3D::Bounds3D(std::array<float, 6> bounds):
            xBounds(bounds[0], bounds[1]),
            yBounds(bounds[2], bounds[3]),
            zBounds(bounds[4], bounds[5]) {}

    Bounds3D::Bounds3D(const EisEngine::Vector3 &bounds):
            xBounds(bounds.x, bounds.x),
            yBounds(bounds.y, bounds.y),
            zBounds(bounds.z, bounds.z)  {

    }

    Bounds3D Bounds3D::Merge(EisEngine::utilities::Bounds3D &A, EisEngine::utilities::Bounds3D &B) {
        return Bounds3D(
                std::min(A.xBounds.x, B.xBounds.x),
                std::max(A.xBounds.y, B.xBounds.y),
                std::min(A.yBounds.x, B.yBounds.x),
                std::max(A.yBounds.y, B.yBounds.y),
                std::min(A.zBounds.x, B.zBounds.x),
                std::max(A.zBounds.y, B.zBounds.y)
        );
    }

    Bounds3D Bounds3D::Expand(EisEngine::utilities::Bounds3D &A, const EisEngine::Vector3 &B) {
        return Bounds3D(
                std::min(A.xBounds.x, B.x),
                std::max(A.xBounds.y, B.x),
                std::min(A.yBounds.x, B.y),
                std::max(A.yBounds.y, B.y),
                std::min(A.zBounds.x, B.z),
                std::max(A.zBounds.y, B.z)
        );
    }

    Bounds3D Bounds3D::Expand(const EisEngine::Vector3 &B) {
        return Expand(*this, B);
    }

    bool Bounds3D::HitByRay(
            const EisEngine::Vector3 &raySource,
            const EisEngine::Vector3 &rayDir,
            float& t_max
    ) const {
        auto inverseDir = Vector3(1/rayDir.x, 1/rayDir.y, 1/rayDir.z);

        auto temp1 = Vector3(xBounds.x, yBounds.x, zBounds.x) - raySource;
        auto tEntry = Vector3(
                temp1.x * inverseDir.x,
                temp1.y * inverseDir.y,
                temp1.z * inverseDir.z
        );

        auto temp2 = Vector3(xBounds.y, yBounds.y, zBounds.y) - raySource;
        auto tExit = Vector3(
                temp2.x * inverseDir.x,
                temp2.y * inverseDir.y,
                temp2.z * inverseDir.z
        );

        auto tLatestEntry = std::max(std::min(tEntry.x, tExit.x), std::max(std::min(tEntry.y, tExit.y), std::min(tEntry.z, tExit.z)));

        // early exit if there IS a hit but it's further than the closest registered hit (performance)
        if (tLatestEntry >= t_max)
            return false;

        auto tEarliestExit = std::min(std::max(tEntry.x, tExit.x), std::min(std::max(tEntry.y, tExit.y), std::max(tEntry.z, tExit.z)));

        // max(tLatestEntry, 0.0f) for culling of rays originating behind the mesh
        return tEarliestExit >= std::max(tLatestEntry, 0.0f);
    }

    Vector3 Bounds3D::GetClosestPointTo(const EisEngine::Vector3 &pos) const {
        // for each dimension if pos.dim in nDim do pos.dim else nDim.x if pos < nDim.x else nDim.y
        float x;
        if (pos.x < xBounds.x)
            x = xBounds.x;
        else if (pos.x > xBounds.y)
            x = xBounds.y;
        else x = pos.x;

        float y;
        if (pos.y < yBounds.x)
            y = yBounds.x;
        else if (pos.y > yBounds.y)
            y = yBounds.y;
        else y = pos.y;

        float z;
        if (pos.z < zBounds.x)
            z = zBounds.x;
        else if (pos.z > zBounds.y)
            z = zBounds.y;
        else z = pos.z;

        return Vector3(x, y, z);
    }

    float Bounds3D::diagonalSize() const {
        return xBounds.magnitude() + yBounds.magnitude() + zBounds.magnitude();
    }

    Vector3 Bounds3D::Centre() const {
        // centre = middle value of every dimension.
        return Vector3(
            (xBounds.x + xBounds.y) / 2,
            (yBounds.x + yBounds.y) / 2,
            (zBounds.x + zBounds.y) / 2
        );
    }
}
