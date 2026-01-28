#include "engine/utilities/Quaternion.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Math.h"
#include <cmath>

namespace EisEngine {
    Quaternion::Quaternion(const EisEngine::Vector3 &axis, const float &r): x(axis.x), y(axis.y), z(axis.z), r(r) {}

    Quaternion Quaternion::FromEulerXYZ(const EisEngine::Vector3 &deg) {
        float sx = sin(Math::DegreesToRadians(deg.x)/2);
        float sy = sin(Math::DegreesToRadians(deg.y)/2);
        float sz = sin(Math::DegreesToRadians(deg.z)/2);
        float cx = cos(Math::DegreesToRadians(deg.x)/2);
        float cy = cos(Math::DegreesToRadians(deg.y)/2);
        float cz = cos(Math::DegreesToRadians(deg.z)/2);

        return Quaternion(
                sx*cy*cz - cx*sy*sz,
                cx*sy*cz + sx*cy*sz,
                cx*cy*sz - sx*sy*cz,
                cx*cy*cz + sx*sy*sz
        );
    }

    const Quaternion Identity = Quaternion(0, 0, 0, 1);

    float Quaternion::magnitude() const
    { return (float) sqrt(pow(r, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2));}

    Quaternion::operator Vector3() const{ return Vector3(x, y, z);}
    Quaternion::operator Vector2() const { return Vector2(x, y);}
    Quaternion::operator std::string() const {
        return "({"+
               std::to_string(x)+", "+
               std::to_string(y)+", "+
               std::to_string(z)+"}, "+
               std::to_string(r)+
               ")";
    }

    Quaternion Quaternion::operator+(const EisEngine::Quaternion &q) const {
        return Quaternion(
                x + q.x,
                y + q.y,
                z + q.z,
                r + q.r
                        );
    }
    Quaternion Quaternion::operator-(const EisEngine::Quaternion &q) const {
        return Quaternion(
                x - q.x,
                y - q.y,
                z - q.z,
                r - q.r
                );
    }
    Quaternion Quaternion::operator*(const EisEngine::Quaternion &q) const {
        return Quaternion(
                r * q.x + x * q.r + y * q.z - z * q.y,
                r * q.y - x * q.z + y * q.r + z * q.x,
                r * q.z + x * q.y - y * q.x + z * q.r,
                r * q.r - x * q.x - y * q.y - z * q.z
                );
    }
    Quaternion Quaternion::operator*(const float &c) const {
        return Quaternion(
                x * c,
                y * c,
                z * c,
                r * c
        );
    }
    Quaternion Quaternion::operator*(const int &c) const {
        return Quaternion(
                x * (float) c,
                y * (float) c,
                z * (float) c,
                r * (float) c
        );
    }

    Vector3 Quaternion::operator*(const Vector3& v) const {
        auto q = Vector3(x, y, z);
        auto t = Vector3::CrossProduct(q, v) * 2;
        return v + r*t + Vector3::CrossProduct(q, t);
    }

    Quaternion &Quaternion::operator+=(const Quaternion &q) {
        x += q.x;
        y += q.y;
        z += q.z;
        r += q.r;
        return *this;
    }
    Quaternion &Quaternion::operator-=(const Quaternion &q) {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        r -= q.r;
        return *this;
    }
    Quaternion &Quaternion::operator*=(const float &c) {
        r *= c;
        x *= c;
        y *= c;
        z *= c;
        return *this;
    }
    Quaternion &Quaternion::operator*=(const int &c) {
        r *= (float) c;
        x *= (float) c;
        y *= (float) c;
        z *= (float) c;
        return *this;
    }

    Quaternion Quaternion::FromAxisAngle(const Vector3 &deg, const float &angle) {
        return Quaternion(
                deg.normalized() * Math::Sin(angle / 2, DEGREES),
                Math::Cos(angle / 2, DEGREES)
            );
    }
}
