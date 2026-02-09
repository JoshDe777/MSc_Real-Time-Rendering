#include "engine/utilities/Quaternion.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Debug.h"
#include "engine/utilities/Math.h"

namespace EisEngine{
    float Vector3::magnitude() const
    { return (float) sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));}

    Vector3 Vector3::normalized() const { return (*this * (1/ this->magnitude()));}

    Vector3 Vector3::ClampMagnitude(const float& minMagnitude, const float& maxMagnitude) {
        float currentMagnitude = magnitude();

        if(currentMagnitude >= minMagnitude && currentMagnitude <= maxMagnitude)
            return *this;                                       // if within clamp range return vector unchanged.
        return currentMagnitude < minMagnitude ?                // if smaller in magnitude than minimum
        this->normalized()*minMagnitude :                       // return the vector scaled to minMagnitude,
        this->normalized()*maxMagnitude;                        // else (larger than max) return scaled to maxMagnitude.
    }

    float Vector3::Distance(const EisEngine::Vector3 &v1, const EisEngine::Vector3 &v2) {
        return Math::Sqrt((v1.x - v2.x) * (v1.x - v2.x) +
                (v1.y - v2.y) * (v1.y - v2.y) +
                (v1.z - v2.z) * (v1.z - v2.z));
    }

    float Vector3::DotProduct(const EisEngine::Vector3 &v1, const EisEngine::Vector3 &v2)
    { return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);}

    Vector3 Vector3::CrossProduct(const EisEngine::Vector3 &v1, const EisEngine::Vector3 &v2) {
        return Vector3(v1.y * v2.z - v1.z * v2.y,
                       v1.z * v2.x - v1.x * v2.z,
                       v1.x * v2.y - v1.y * v2.x);
    }

    float Vector3::Angle(const EisEngine::Vector3 &v1, const EisEngine::Vector3 &v2) {
        if(v1.magnitude() == 0 || v2.magnitude() == 0){
            DEBUG_ERROR("Trying to calculate angle with 0 vector!")
            return 0;
        }
        auto angleCos = DotProduct(v1, v2) / (v1.magnitude() * v2.magnitude());
        auto angleSin = CrossProduct(v1, v2).magnitude() / (v1.magnitude() * v2.magnitude());
        return Math::Arctan(angleSin, angleCos);
    }

    Vector3::operator Vector2() const { return Vector2(x, y);}
    Vector3::operator Quaternion() const { return Quaternion(x, y, z);}
    Vector3::operator glm::vec3() const { return {x, y, z};}
    Vector3::operator glm::vec2() const { return {x, y};}
    Vector3::operator b2Vec2() const { return {x, y};}
    Vector3::operator std::string() const
    { return "("+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z)+")";}

    Vector3 Vector3::operator-() const { return Vector3(-x, -y, -z);}
    Vector3 Vector3::operator+(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z);}
    Vector3 Vector3::operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z);}
    Vector3 Vector3::operator*(const float &c) const { return Vector3(c*x, c*y, c*z);}
    Vector3 operator*(float scalar, const Vector3& v) { return v*scalar;}
    Vector3 Vector3::operator*(const int &c) const { return Vector3((float) c*x, (float) c*y, (float) c*z);}
    Vector3 operator*(int scalar, const Vector3& v) { return v*scalar;}

    Vector3 Vector3::operator*(const glm::mat4& m) const {
        glm::vec4 v(x, y, z, 1.0f);
        glm::vec4 transformedVector = m * v;
        return Vector3(transformedVector.x, transformedVector.y, transformedVector.z);
    }

    Vector3 &Vector3::operator+=(const EisEngine::Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3 &Vector3::operator-=(const EisEngine::Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Vector3 &Vector3::operator*=(const float &c) {
        x *= c;
        y *= c;
        z *= c;
        return *this;
    }
    Vector3 &Vector3::operator*=(const int &c) {
        x *= (float) c;
        y *= (float) c;
        z *= (float) c;
        return *this;
    }

    bool Vector3::operator==(const EisEngine::Vector2 &v) const {return x == v.x && y == v.y;}

    const Vector3 Vector3::zero = Vector3(0, 0, 0);
    const Vector3 Vector3::one = Vector3(1, 1, 1);
    const Vector3 Vector3::forward = Vector3(0, 0, -1);
    const Vector3 Vector3::right = Vector3(1, 0, 0);
    const Vector3 Vector3::up = Vector3(0, 1, 0);

    Vector3 Vector3::Rotate(const EisEngine::Vector3 &rotationAngles) const {
        // map to quaternion-based rotation.
        return Rotate(Quaternion::FromEulerXYZ(rotationAngles));
    }

    Vector3 Vector3::Rotate(const EisEngine::Quaternion &q) const {
        auto vQuat = Quaternion(x, y, z, 0);
        auto invQ = q.conjugated();

        auto result = q * vQuat * invQ;
        return (Vector3) result;
    }
}
