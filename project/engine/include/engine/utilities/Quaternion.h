#pragma once

#include <string>
#include <assimp/quaternion.h>

namespace EisEngine {
    class Vector3;
    class Vector2;

    /// [Not yet implemented into core features] EisEngine's very own Quaternions!
    class Quaternion {
    public:
        /// \n Creates a new Quaternion
        /// @param x: The x value of this quaternion
        /// @param y: The y value of this quaternion
        /// @param z: The z value of this quaternion
        /// @param r: The r value of this quaternion
        explicit Quaternion(float x = 0, float y = 0, float z = 0, float r = 0): x(x), y(y), z(z), r(r) {}
        /// \n Creates a new Quaternion from an aiQuaternion object.
        /// @param q - aiQuaternion: the quaternion data used to create this quaternion.
        explicit Quaternion(const aiQuaternion& q) : x(q.x), y(q.y), z(q.z), r(q.w) {}

        /// \n The x value of this quaternion
        float x;
        /// \n The y value of this quaternion
        float y;
        /// \n The z value of this quaternion
        float z;
        /// \n The r value of this quaternion
        float r;

        operator Vector3() const;
        operator Vector2() const;
        operator std::string() const;

        Quaternion operator+(Quaternion const &q) const;
        Quaternion operator-(Quaternion const &q) const;
        Quaternion operator*(Quaternion const &q) const;
        Quaternion operator*(float const& c) const;
        Quaternion operator*(int const& c) const;

        Quaternion& operator+=(Quaternion const &q);
        Quaternion& operator-=(Quaternion const &q);
        Quaternion & operator*=(float const &c);
        Quaternion & operator*=(int const &c);

        /// A quaternion providing no rotation\n -> (0, 0, 0, 1)
        static const Quaternion Identity;

        /// The absolute scale of the quaternion.
        /// @return a float representing the absolute value of the quaternion.
        float magnitude() const;

        /// The quaternion normalized to have the sum of its members squared be 1.
        Quaternion normalized() { return (*this * (1/this->magnitude()));}

        /// The conjugated value of this quaternion.
        Quaternion conjugated() { return Quaternion(r, -x, -y, -z);}
    };
}
