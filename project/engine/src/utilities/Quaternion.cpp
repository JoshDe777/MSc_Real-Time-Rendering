#include "engine/utilities/Quaternion.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/Vector2.h"
#include "engine/utilities/Math.h"
#include <cmath>

namespace EisEngine {
    Quaternion::Quaternion(const EisEngine::Vector3 &axis, const float &r): x(axis.x), y(axis.y), z(axis.z), r(r) {}

    Quaternion Quaternion::FromEulerXYZ(const EisEngine::Vector3 &deg) {
        float sinx = sin(deg.x / 2);
        float siny = sin(deg.y / 2);
        float sinz = sin(deg.z / 2);
        float cosx = cos(deg.x / 2);
        float cosy = cos(deg.y / 2);
        float cosz = cos(deg.z / 2);

        return Quaternion(
                sinx * cosy * cosz - cosx * siny * sinz,
                cosx * siny * cosz + sinx * cosy * sinz,
                cosx * cosy * sinz - sinx * siny * cosz,
                cosx * cosy * cosz + sinx * siny * sinz
        ).normalized();
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

    Quaternion Quaternion::operator-() const { return Quaternion(-x, -y, -z, -r);}
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
        // negate z values because y and z axes flip for some reason.
        auto vTemp = Vector3(v.x, v.y, -v.z);
        auto q = Vector3(x, y, z);
        auto t = Vector3::CrossProduct(q, vTemp) * 2;
        auto mRes = vTemp + r*t + Vector3::CrossProduct(q, t);
        return Vector3(mRes.x, mRes.y, mRes.z);
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

    // math derived from David Parker
    // "Converting quaternions to Euler angles - Programming TIL #197 3D Math 41 tutorial video screencast"
    // on Youtube, link: https://youtu.be/vxPVw_EgyJI?si=EsirZdPvnlu927FE
    Vector3 Quaternion::ToEulerXYZ() const {
        auto pitch = Math::Arcsin(-2 * (y*z + r*x), DEGREES);
        auto cosP = Math::Cos(pitch, DEGREES);
        // gimbal lock bypassing
        // case no GL
        float yaw;
        float roll;
        if(cosP != 0){
            yaw = Math::Arctan(2*x*z - 2*r*y, 1.0f-2*pow(x, 2)-2*pow(y,2), DEGREES);
            roll = Math::Arctan(x*y-r*z, 0.5f-pow(x,2)-pow(z,2));
        }
        else{
            yaw = Math::Arctan(-x*z-r*y, 0.5f-pow(y,2)-pow(z,2), DEGREES);
            roll = 0.0f;
        }
        return Vector3(pitch, yaw, roll);
    }

    float Quaternion::Dot(const EisEngine::Quaternion &q1, const EisEngine::Quaternion &q2) {
        return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.r * q2.r;
    }

    Quaternion Quaternion::Lerp(const EisEngine::Quaternion &q1, const EisEngine::Quaternion &q2, const float &val) {
        // avoid interpolating angles > 180
        auto tempQ2 = Dot(q1, q2) < 0.0f ? q2 : -q2;

        return Quaternion(
            Math::Lerp(q1.x, tempQ2.x, val),
            Math::Lerp(q1.y, tempQ2.y, val),
            Math::Lerp(q1.z, tempQ2.z, val),
            Math::Lerp(q1.r, tempQ2.r, val)
        );
    }

    Quaternion Quaternion::GetRotationToTarget(const EisEngine::Vector3 &origin, const EisEngine::Vector3 &target) {
        auto o = origin.normalized();
        auto t = target.normalized();

        // no rotation needed if exact match.
        if(o == t)
            return Quaternion(0, 0, 0, 1);

        float cosAngle = Vector3::DotProduct(o, t);

        // catch edge case of diametrically opposed vectors (cross product = (0,0,0))
        if(cosAngle == -1){
            auto planeAxis = Vector3::CrossProduct(Vector3::up, o);
            // edge-case-ception: if the original vector is also Vector3::up
            if(pow(planeAxis.magnitude(), 2) < b2_epsilon)
                planeAxis = Vector3::CrossProduct(Vector3::right, o);

            return Quaternion::FromAxisAngle(planeAxis.normalized(), 180);
        }

        auto planeAxis = Vector3::CrossProduct(o, t);
        // half-angle identity calculation -> ( cos(a/2) = sqrt((1+cos(a))/2) )
        // *2 because quat x quat is angle/2 x angle/2
        auto hAI = Math::Sqrt((1.0f + cosAngle) * 2.0f);
        float hAIinv = 1.0f / hAI;

        return Quaternion(
            planeAxis.x * hAIinv,
            planeAxis.y * hAIinv,
            planeAxis.z * hAIinv,
            hAI / 2
        ).normalized();
    }
}
