#pragma once

#include <cmath>

namespace EisEngine {
    /// \n Makes a distinction between degrees and radians for function clarity.
    enum AngleType {
        DEGREES,
        RADIANS
    };

    /// \n Provides convenient math and angle operations in one static class.
    class Math {
    public:
        /// \n The iconic pi.
        constexpr static const float PI = 3.14159265358979323846f;

        /// \n Returns the cos value of the given angle
        /// @param angle - float: the angle in the given angle type.
        /// @param angleType - AngleType: the angle type. Defaults to radians.
        static float Cos(const float& angle, AngleType angleType = RADIANS);

        /// \n Returns the angle assigned to the provided cos value.
        /// @param cosVal - float: the cos value of the desired angle.
        /// @param angleType - AngleType: the angle type. Defaults to radians.
        static float Arccos(const float& cosVal, AngleType angleType = RADIANS);

        /// \n Returns the sin value of the given angle
        /// @param angle - float: the angle in the given angle type.
        /// @param angleType - AngleType: the angle type. Defaults to radians.
        static float Sin(const float& angle, AngleType angleType = RADIANS);

        /// \n Returns the angle assigned to the provided sin value.
        /// @param sinVal - float: the sin value of the desired angle.
        /// @param angleType - AngleType: the angle type. Defaults to radians.
        static float Arcsin(const float& sinVal, AngleType angleType = RADIANS);

        /// \n Calculates the tangent value of the provided angle.
        /// @param angle - float: the angle in the given angle type.
        /// @param angleType - AngleType: the angle type. Defaults to radians.
        static float Tan(const float& angle, AngleType angleType = RADIANS);

        /// \n Calculates the angle from the given sin and cos values.
        /// @param sinVal - float: the angle's sin value.
        /// @param cosVal - float: the angles cos value.
        /// @param angleType - the angle type. Defaults to radians.
        static float Arctan(const float& sinVal, const float& cosVal, AngleType angleType = RADIANS);

        /// \n Converts an angle from degrees to radians.
        /// @param angle - float: the angle in degrees.
        static float DegreesToRadians(const float& angle);

        /// \n Converts an angle from radians to degrees.
        /// @param angle - float: the angle in radians.
        static float RadiansToDegrees(const float& angle);

        /// \n Clamps the given float to fit between the provided min and max values.
        /// @param floatToClamp - float: the value to be clamped.
        /// @param minValue - float: the minimum value the variable should be allowed to take on.
        /// @param maxValue - float: the maximum value the variable should be allowed to take on.
        static float Clamp(const float& floatToClamp, const float& minValue, const float& maxValue);

        /// \n rounds the given value down to the next integer.
        static float Floor(const float& val);

        /// \n Returns the distance between two floats.
        static float Dist(const float& val1, const float& val2);

        /// \n unsigned modulo function mapping the provided value to the interval [0, modVal[.
        static float Mod(const float& val1, const float& modVal);

        /// \n Calculates the square root of the given value.
        static float Sqrt(const float& val);

        static float Lerp(const float& start, const float& end, const float& t);
    };
}
