#include <algorithm>
#include "engine/utilities/Math.h"

namespace EisEngine {
    float Math::Cos(const float &angle, AngleType angleType)
    { return cos(angleType == RADIANS ? angle : DegreesToRadians(angle));}

    float Math::Arccos(const float &cosVal, EisEngine::AngleType angleType) {
        auto finalAngle = acos(cosVal);
        return angleType == RADIANS ? finalAngle : RadiansToDegrees(finalAngle);
    }

    float Math::Sin(const float &angle, EisEngine::AngleType angleType)
    { return sin(angleType == RADIANS ? angle : DegreesToRadians(angle));}

    float Math::Arcsin(const float &sinVal, EisEngine::AngleType angleType) {
        auto finalAngle = asin(sinVal);
        return angleType == RADIANS ? finalAngle : RadiansToDegrees(finalAngle);
    }

    float Math::Tan(const float &angle, EisEngine::AngleType angleType)
    { return tan(angleType == RADIANS ? angle : DegreesToRadians(angle));}

    float Math::Arctan(const float &sinVal, const float &cosVal, EisEngine::AngleType angleType) {
        return angleType == RADIANS ?
        atan2(sinVal, cosVal) :
        RadiansToDegrees(atan2(sinVal, cosVal));
    }

    float Math::DegreesToRadians(const float& angle) { return angle * PI / 180.0f;}

    float Math::RadiansToDegrees(const float &angle) { return angle * 180 / PI;}

    float Math::Clamp(const float &floatToClamp, const float &minValue, const float &maxValue)
    { return std::clamp(floatToClamp, minValue, maxValue);}

    float Math::Floor(const float &val) { return floor(val);}

    float Math::Dist(const float &val1, const float &val2) { return abs(val1 - val2);}

    float Math::Mod(const float &val1, const float &modVal) {
        auto result = std::fmod(val1, modVal);
        if(result < 0)
            result +=modVal;
        return result;
    }

    float Math::Sqrt(const float &val) { return std::sqrt(val);}
}
