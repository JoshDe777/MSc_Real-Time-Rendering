#include "engine/utilities/Quaternion.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Math.h"
#include "engine/utilities/Debug.h"

namespace EisEngine{
    float Vector2::magnitude() const { return (float) sqrt(pow(x, 2) + pow(y, 2));}

    Vector2 Vector2::normalized() const { return (*this * (1/ this->magnitude()));}

    Vector2 Vector2::ClampMagnitude(const float& minMagnitude, const float& maxMagnitude) {
        float currentMagnitude = magnitude();
        // if within clamp range return vector unchanged.
        if(currentMagnitude >= minMagnitude && currentMagnitude <= maxMagnitude)
            return *this;
        // if smaller in magnitude than minimum, return the vector scaled to minMagnitude,
        // else (greater than max) return scaled to maxMagnitude.
        return (currentMagnitude < minMagnitude ? this->normalized()*minMagnitude : this->normalized()*maxMagnitude);
    }

    float Vector2::Distance(const EisEngine::Vector2 &v1, const EisEngine::Vector2 &v2)
    { return Math::Sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));}

    float Vector2::DotProduct(const EisEngine::Vector2 &v1, const EisEngine::Vector2 &v2)
    { return (v1.x * v2.x) + (v1.y * v2.y);}

    float Vector2::CrossProduct(const EisEngine::Vector2 &v1, const EisEngine::Vector2 &v2)
    { return (v1.x * v2.y) - (v1.y * v2.x);}

    float Vector2::Angle(const EisEngine::Vector2 &v1, const EisEngine::Vector2 &v2) {
        if(v1.magnitude() == 0 || v2.magnitude() == 0){
            DEBUG_ERROR("Trying to calculate angle using a zero vector!")
            return 0;
        }
        return Math::Arctan(CrossProduct(v1, v2), DotProduct(v1, v2));
    }

    Vector2 Vector2::Rotate(const float& angle) const {
        float angleCos = Math::Cos(angle, DEGREES);
        float angleSin = Math::Sin(angle, DEGREES);

        float rotatedX = x * angleCos - y * angleSin;
        float rotatedY = x * angleSin + y * angleCos;

        return Vector2(rotatedX, rotatedY);
    }

    Vector2::operator Vector3() const { return Vector3(x, y, 0);}
    Vector2::operator Quaternion() const{ return Quaternion(x, y);}
    Vector2::operator glm::vec3() const { return {x, y, 0};}
    Vector2::operator glm::vec2() const { return {x, y};}
    Vector2::operator b2Vec2() const { return {x, y};}
    Vector2::operator std::string() const { return "("+std::to_string(x)+", "+std::to_string(y)+")";}

    Vector2 Vector2::operator-() const {return Vector2(-x, -y);}
    Vector2 Vector2::operator+(const EisEngine::Vector2 &v) const { return Vector2(x + v.x, y + v.y);}
    Vector2 Vector2::operator-(const EisEngine::Vector2 &v) const {return Vector2(x - v.x, y - v.y);}
    Vector2 Vector2::operator*(const int &c) const {return Vector2(x * (float) c, y * (float) c);}
    Vector2 operator*(float scalar, const Vector2& v) { return v*scalar;}
    Vector2 Vector2::operator*(const float &c) const { return Vector2(c*x, c*y);}
    Vector2 operator*(int scalar, const Vector2& v) { return v*scalar;}
    Vector2 Vector2::operator/(const int &c) const {return Vector2(x / (float) c, y / (float) c);}
    Vector2 operator/(float scalar, const Vector2& v) { return v / scalar;}
    Vector2 Vector2::operator/(const float &c) const { return Vector2(x / c, y / c);}
    Vector2 operator/(int scalar, const Vector2& v) { return v / scalar;}

    Vector2 &Vector2::operator+=(const EisEngine::Vector2 &v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2 &Vector2::operator-=(const EisEngine::Vector2 &v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vector2 &Vector2::operator*=(const float &c) {
        x *= c;
        y *= c;
        return *this;
    }
    Vector2 &Vector2::operator*=(const int &c) {
        x *= (float) c;
        y *= (float) c;
        return *this;
    }

    bool Vector2::operator==(const EisEngine::Vector2 &v) const { return x == v.x && y == v.y;}
    bool Vector2::operator!=(const EisEngine::Vector2 &v) const { return x != v.x || y != v.y;}

    const Vector2 Vector2::zero = Vector2(0, 0);
    const Vector2 Vector2::one = Vector2(1, 1);
    const Vector2 Vector2::up = Vector2(0, 1);
    const Vector2 Vector2::right = Vector2(1, 0);
}
