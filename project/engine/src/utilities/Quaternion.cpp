#include "engine/utilities/Quaternion.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/Vector2.h"
#include <cmath>

namespace EisEngine {
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
                r * q.x + q.r * x + y * q.z - z * q.y,
                r * q.y - q.r * y + z * q.x - x * q.z,
                r * q.z - q.r * z + x * q.y - y * q.x,
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
}
